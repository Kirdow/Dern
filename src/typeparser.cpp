#include "pch.h"
#include "typeparser.h"
#include "token.h"

namespace Dern
{
    Ref<StoredValue> TypeParser::ComputeValue(std::function<Ref<Token>(ParseMem&)> nextFn)
    {
        std::vector<Ref<ParseToken>> tokens;

        Ref<Token> token = nullptr;
        while ((token = nextFn(m_Mem)) != nullptr)
        {
            if (token->IsType(TokenType::Sym))
                tokens.push_back(Ref<SymToken>::Create(token->GetData()));
            else if (token->IsType(TokenType::Int))
            {
                int numData = token->GetDataInt();
                tokens.push_back(Ref<NumberToken>::Create(numData));
            }
            else if (token->IsType(TokenType::Name))
            {
                std::string varName = token->GetData();
                tokens.push_back(Ref<VarToken>::Create(varName));
            }
            else if (token->IsType(TokenType::Text))
            {
                std::string strData = token->GetData();
                tokens.push_back(Ref<TextToken>::Create(strData));
            }
        }

        auto result = ComputeValue(tokens);
        if (result)
        {
            if (result->IsType(PTokenType::Int))
            {
                int data = result->Cast<NumberToken>()->Value;
                auto ref = Ref<StoredValue>::Create();
                ref->SetData<int>(data);
                return ref;
            }
            else if (result->IsType(PTokenType::Text))
            {
                std::string data = result->Cast<TextToken>()->Value;
                auto ref = Ref<StoredValue>::Create();
                ref->SetData<std::string>(data);
                return ref;
            }
        }

        return Ref<StoredValue>::Create();
    }

    static bool ValidMDASToken(const Ref<ParseToken>& token)
    {
        return token->IsType(PTokenType::Int) || token->IsType(PTokenType::Var);
    }

    static int GetMDASValue(const Ref<ParseToken>& token, const Registry& reg)
    {
        if (token->IsType(PTokenType::Int)) return token->Cast<NumberToken>()->Value;
        else if (token->IsType(PTokenType::Var))
        {
            auto name = token->Cast<VarToken>()->Value;
            if (!reg.HasEntry<int>(name))
                throw "Unexpected variable name";

            return reg.GetEntry<int>(name);
        }

        throw "Unexpected MDAS token type";
    }

    Ref<ParseToken> TypeParser::ComputeValue(std::vector<Ref<ParseToken>> v)
    {
        if (v.size() == 0)
            return Ref<ParseToken>::Create(PTokenType::None);

        if (v.size() == 1)
        {
            auto token = v[0];

            if (token->IsType(PTokenType::Int))
            {
                int result = token->Cast<NumberToken>()->Value;

                return Ref<NumberToken>::Create(result);
            }
            else if (token->IsType(PTokenType::Text))
            {
                std::string result = token->Cast<TextToken>()->Value;

                return Ref<TextToken>::Create(result);
            }
            else if (token->IsType(PTokenType::Var))
            {
                auto name = token->Cast<VarToken>()->Value;
                if (m_Reg.HasEntry<int>(name))
                {
                    int result = m_Reg.GetEntry<int>(name);

                    return Ref<NumberToken>::Create(result);
                }
                else if (m_Reg.HasEntry<std::string>(name))
                {
                    std::string result = m_Reg.GetEntry<std::string>(name);

                    return Ref<TextToken>::Create(result);
                }
            }
            
            return Ref<ParseToken>::Create(PTokenType::None);
        }

        std::vector<Ref<ParseToken>> tmp;

        // ( )
        while (true)
        {
            tmp = v;
            
            for (size_t i = 0; i < v.size(); i++)
            {
                auto token = v.at(i);
                if (!token->IsType(PTokenType::Sym) || token->Cast<SymToken>()->Value != "(")
                    continue;

                int mem = 0;
                int result = -1;
                for (size_t j = i + 1; j < v.size(); j++)
                {
                    token = v.at(j);
                    if (token->IsType(PTokenType::Sym))
                    {
                        if (token->Cast<SymToken>()->Value == "(")
                        {
                            ++mem;
                        }
                        else if (token->Cast<SymToken>()->Value == ")")
                        {
                            if (mem <= 0)
                            {
                                result = static_cast<int>(j);
                                break;
                            }

                            --mem;
                        }
                    }
                }

                if (result < 0)
                    throw "Expected ')'";

                std::vector<Ref<ParseToken>> partVec(v.begin() + i + 1, v.begin() + result);

                auto recursiveResult = ComputeValue(partVec);
                v.erase(v.begin() + i, v.begin() + result);
                v[i] = recursiveResult;

                break;
            }

            if (v.size() == tmp.size())
                break;
        }

        // * /
        while (true)
        {
            tmp = v;

            for (size_t i = 1; i + 1 < v.size(); i += 2)
            {
                auto token = v.at(i);
                if (!token->IsType(PTokenType::Sym))
                    throw "Unexpected token";

                auto sym = token->Cast<SymToken>()->Value;
                if (sym == "*" || sym == "/")
                {
                    auto left = v.at(i - 1);
                    auto right = v.at(i + 1);

                    if (!ValidMDASToken(left) || !ValidMDASToken(right))
                        throw "Unexpected token type for MD";
                    
                    auto leftInt = GetMDASValue(left, m_Reg);
                    auto rightInt = GetMDASValue(right, m_Reg);

                    int result = 0;
                    if (sym == "*") result = leftInt * rightInt;
                    else result = leftInt / rightInt;

                    auto ref = Ref<NumberToken>::Create(result);
                    v[i] = ref;
                    v.erase(v.begin() + i + 1);
                    v.erase(v.begin() + i - 1);
                    break;
                }
            }

            if (v.size() == tmp.size())
                break;
        }

        // + -
        while (true)
        {
            tmp = v;

            for (size_t i = 1; i + 1 < v.size(); i += 2)
            {
                auto token = v.at(i);
                if (!token->IsType(PTokenType::Sym))
                    throw "Unexpected token";

                auto sym = token->Cast<SymToken>()->Value;
                if (sym == "+" || sym == "-")
                {
                    auto left = v.at(i - 1);
                    auto right = v.at(i + 1);

                    bool leftValidForText = (left->IsType(PTokenType::Text) || (left->IsType(PTokenType::Var) && m_Reg.HasEntry<std::string>(left->Cast<VarToken>()->Value)));
                    bool rightValidForText = (right->IsType(PTokenType::Text) || (right->IsType(PTokenType::Var) && m_Reg.HasEntry<std::string>(right->Cast<VarToken>()->Value)));
                    if (leftValidForText || rightValidForText)
                    {
                        if (sym != "+")
                            throw "Unexpected symbol, expected '+'";

                        std::stringstream sstr;
                        if (left->IsType(PTokenType::Text))
                        {
                            auto leftStr = left->Cast<TextToken>()->Value;
                            sstr << leftStr;
                        }
                        else if (left->IsType(PTokenType::Int))
                        {
                            auto leftInt = left->Cast<NumberToken>()->Value;
                            sstr << leftInt;
                        }
                        else if (left->IsType(PTokenType::Var))
                        {
                            auto name = left->Cast<VarToken>()->Value;
                            if (m_Reg.HasEntry<int>(name))
                            {
                                auto leftInt = m_Reg.GetEntry<int>(name);

                                sstr << leftInt;
                            }
                            else if (m_Reg.HasEntry<std::string>(name))
                            {
                                auto leftStr = m_Reg.GetEntry<std::string>(name);

                                sstr << leftStr;
                            }
                            else
                                throw "Invalid text variable";
                        }
                        else
                            throw "Unexpected token";

                        if (right->IsType(PTokenType::Text))
                        {
                            auto rightStr = right->Cast<TextToken>()->Value;
                            sstr << rightStr;
                        }
                        else if (right->IsType(PTokenType::Int))
                        {
                            auto rightInt = right->Cast<NumberToken>()->Value;
                            sstr << rightInt;
                        }
                        else if (right->IsType(PTokenType::Var))
                        {
                            auto name = right->Cast<VarToken>()->Value;
                            if (m_Reg.HasEntry<int>(name))
                            {
                                auto rightInt = m_Reg.GetEntry<int>(name);

                                sstr << rightInt;
                            }
                            else if (m_Reg.HasEntry<std::string>(name))
                            {
                                auto rightStr = m_Reg.GetEntry<std::string>(name);

                                sstr << rightStr;
                            }
                            else
                                throw "Invalid text variable";
                        }
                        else
                            throw "Unexpected text token";

                        auto ref = Ref<TextToken>::Create(sstr.str());
                        v[i] = ref;
                        v.erase(v.begin() + i + 1);
                        v.erase(v.begin() + i - 1);
                        break;
                    }

                    if (!ValidMDASToken(left) || !ValidMDASToken(right))
                        throw "Unexpected token type for AS";
                    
                    auto leftInt = GetMDASValue(left, m_Reg);
                    auto rightInt = GetMDASValue(right, m_Reg);

                    int result = 0;
                    if (sym == "+") result = leftInt + rightInt;
                    else result = leftInt - rightInt;

                    auto ref = Ref<NumberToken>::Create(result);
                    v[i] = ref;
                    v.erase(v.begin() + i + 1);
                    v.erase(v.begin() + i - 1);
                    break;
                }
            }

            if (v.size() == tmp.size())
                break;
        }

        if (v.size() != 1) throw "Unexpected token parse result size!";

        return v[0];
        
    }
}