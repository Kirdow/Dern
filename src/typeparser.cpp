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
                tokens.push_back(CreateRef<SymToken>(token->GetData()));
            else if (token->IsType(TokenType::Value))
            {
                std::string data = token->GetData();
                int numData = std::stoi(data);
                tokens.push_back(CreateRef<NumberToken>(numData));
            }
            else if (token->IsType(TokenType::Name))
            {
                std::string varName = token->GetData();
                tokens.push_back(CreateRef<VarToken>(varName));
            }
            else
                throw "Not implemented!";
        }

        auto result = ComputeValue(tokens);
        if (result && result->IsType(PTokenType::Int))
        {
            int data = result->Cast<NumberToken>()->Value;
            auto ref = CreateRef<StoredValue>();
            ref->SetData<int>(data);
            return ref;
        }

        return CreateRef<StoredValue>();
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

                    auto ref = CreateRef<NumberToken>(result);
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

                    if (!ValidMDASToken(left) || !ValidMDASToken(right))
                        throw "Unexpected token type for AS";
                    
                    auto leftInt = GetMDASValue(left, m_Reg);
                    auto rightInt = GetMDASValue(right, m_Reg);

                    int result = 0;
                    if (sym == "+") result = leftInt + rightInt;
                    else result = leftInt - rightInt;

                    auto ref = CreateRef<NumberToken>(result);
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