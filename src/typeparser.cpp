#include "typeparser.h"
#include "token.h"
#include <regex>

namespace Dern
{
    Ref<StoredValue> TypeParser::ComputeValue(std::function<Ref<Token>(ParseMem&)> nextFn)
    {
        std::vector<Ref<ParseToken>> tokens;

        DEBUG_LOG("TypeParser::Collect");
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

        DEBUG_LOG("TypeParser::Compute");
        auto result = ComputeValue(tokens);
        DEBUG_LOG("TypeParser::Computed");
        if (result)
        {
            if (result->IsType(PTokenType::Int))
            {
                int data = result->Cast<NumberToken>()->Value;
                auto ref = Ref<StoredValue>::Create();
                ref->SetData<int>(data);
                DEBUG_LOG("TypeParser::Result::Int(" << data << ")");
                return ref;
            }
            else if (result->IsType(PTokenType::Text))
            {
                std::string data = result->Cast<TextToken>()->Value;
                auto ref = Ref<StoredValue>::Create();
                ref->SetData<std::string>(data);
                DEBUG_LOG("TypeParser::Result::Text(" << data << ")");
                return ref;
            }
        }

        DEBUG_LOG("TypeParser::Result::None");
        return Ref<StoredValue>::Create();
    }

    static bool ValidMDASToken(const Ref<ParseToken>& token)
    {
        return token->IsType(PTokenType::Int) || token->IsType(PTokenType::Var);
    }

    static int GetMDASValue(const Ref<ParseToken>& token, const Ref<Registry>& reg)
    {
        if (token->IsType(PTokenType::Int)) return token->Cast<NumberToken>()->Value;
        else if (token->IsType(PTokenType::Var))
        {
            auto name = token->Cast<VarToken>()->Value;
            if (!reg->HasEntry<int>(name))
                throw "Unexpected variable name";

            return reg->GetEntry<int>(name);
        }

        throw "Unexpected MDAS token type";
    }

    Ref<ParseToken> TypeParser::ComputeValue(std::vector<Ref<ParseToken>> v)
    {
        if (v.size() == 0)
        {
            DEBUG_LOG("TypeParser(Input is empty, returning None)");
            return Ref<ParseToken>::Create(PTokenType::None);
        }

        if (v.size() == 1)
        {
            DEBUG_LOG("TypeParser(Input has one element)");
            auto token = v[0];

            if (token->IsType(PTokenType::Int))
            {
                int result = token->Cast<NumberToken>()->Value;
                DEBUG_LOG("Type::Int(" << result << ")");

                return Ref<NumberToken>::Create(result);
            }
            else if (token->IsType(PTokenType::Text))
            {
                std::string result = token->Cast<TextToken>()->Value;
                DEBUG_LOG("Type::Text(" << result << ")");

                return Ref<TextToken>::Create(result);
            }
            else if (token->IsType(PTokenType::Var))
            {
                auto name = token->Cast<VarToken>()->Value;
                DEBUG_LOG("Result::Var(" << name << ")");
                if (m_Sys->GetRegistry()->HasEntry<int>(name))
                {
                    int result = m_Sys->GetRegistry()->GetEntry<int>(name);
                    DEBUG_LOG("Type::Int(" << result << ")");

                    return Ref<NumberToken>::Create(result);
                }
                else if (m_Sys->GetRegistry()->HasEntry<std::string>(name))
                {
                    std::string result = m_Sys->GetRegistry()->GetEntry<std::string>(name);
                    DEBUG_LOG("Type::Text(" << result << ")");

                    return Ref<TextToken>::Create(result);
                }
            }
            
            DEBUG_LOG("Type::None");
            return Ref<ParseToken>::Create(PTokenType::None);
        }

        #ifdef ENABLE_DEBUG_LOG

        int index = 0;
        for (auto& val : v)
        {
            DEBUG_LOG("Val[" << index << "]: " << "\"" << *val << "\"");
            ++index;
        }

        #endif

        std::vector<Ref<ParseToken>> tmp;

        DEBUG_LOG("PARSING ( )");
        // ( )
        while (true)
        {
            tmp = v;
            
            for (size_t i = 0; i < v.size(); i++)
            {
                auto token = v.at(i);
                if (!token->IsType(PTokenType::Sym) || token->Cast<SymToken>()->Value != "(")
                    continue;

                if (i > 0)
                {
                    auto token0 = v.at(i - 1);
                    if (token0->IsType(PTokenType::Var))
                    {
                        std::string funcName = token0->Cast<VarToken>()->Value;

                        std::vector<Ref<StoredValue>> params;
                        int index = i + 1;
                        int indexEnd = index;
                        while (true)
                        {
                            bool isComma = false;
                            int endIndex = index;
                            int mem = 0;
                            while (endIndex < static_cast<int>(v.size()))
                            {
                                auto token1 = v.at(endIndex);
                                if (token1->IsType(PTokenType::Sym) && token1->Cast<SymToken>()->Value == "(")
                                {
                                    ++mem;
                                }
                                else if (token1->IsType(PTokenType::Sym) && token1->Cast<SymToken>()->Value == ")")
                                {
                                    if (mem <= 0) break;
                                    --mem;
                                }
                                else if (token1->IsType(PTokenType::Sym) && token1->Cast<SymToken>()->Value == ",")
                                {
                                    if (mem <= 0)
                                    {
                                        isComma = true;
                                        break;
                                    }
                                }

                                ++endIndex;
                            }

                            std::vector<Ref<ParseToken>> partVec(v.begin() + index, v.begin() + endIndex);
                            if (partVec.size() == 0)
                            {
                                if (!isComma && params.size() == 0)
                                {
                                    indexEnd = endIndex;
                                    break;
                                }

                                throw "Unexpected ')'. Expected expression!";
                            }
                            DEBUG_LOG("Found param");
                            auto paramResult = ComputeValue(partVec);
                            if (!paramResult) throw "Failed to parse param";

                            auto ref = Ref<StoredValue>::Create();
                            if (paramResult->IsType(PTokenType::Int))
                            {
                                int data = paramResult->Cast<NumberToken>()->Value;
                                DEBUG_LOG("TypeParser::CallParam::Int(" << data << ")");
                                ref->SetData<int>(data);
                                params.push_back(ref);
                            }
                            else if (paramResult->IsType(PTokenType::Text))
                            {
                                std::string data = paramResult->Cast<TextToken>()->Value;
                                DEBUG_LOG("TypeParser::CallParam::Text(" << data << ")");
                                ref->SetData<std::string>(data);
                                params.push_back(ref);
                            }
                            else
                                throw "Unknown param type";

                            index = endIndex + 1;
                            if (!isComma)
                            {
                                indexEnd = endIndex;
                                break;
                            }
                        }

                        if (funcName == "read")
                        {
                            if (params.size() >= 1)
                            {
                                auto precedingValue = params.at(0);
                                if (precedingValue->IsOfType<std::string>())
                                {
                                    auto str = precedingValue->GetData<std::string>();
                                    std::cout << str;
                                }
                                else
                                    throw "Invalid parameter";
                            }


                            std::string line;
                            while (line.length() == 0)
                                std::getline(std::cin, line);
                            
                            Ref<ParseToken> tokenResult = nullptr;
                            std::regex numberRegex("\\-?[0-9]+", std::regex_constants::ECMAScript);
                            std::smatch sm;
                            if (std::regex_search(line, sm, numberRegex) && !sm.prefix().matched && !sm.suffix().matched)
                            {
                                int num = std::stoi(line);
                                tokenResult = Ref<NumberToken>::Create(num);
                            }
                            else
                            {
                                tokenResult = Ref<TextToken>::Create(line);
                            }

                            v.erase(v.begin() + i - 1, v.begin() + indexEnd);
                            v[i - 1] = tokenResult;
                            break;
                        }

                        auto result = m_Sys->Call(funcName, params);
                        Ref<ParseToken> tokenResult = nullptr;
                        if (result->IsOfType<int>())
                        {
                            int data = result->GetData<int>();
                            tokenResult = Ref<NumberToken>::Create(data);
                        }
                        else if (result->IsOfType<std::string>())
                        {
                            std::string data = result->GetData<std::string>();
                            tokenResult = Ref<TextToken>::Create(data);
                        }
                        else
                            throw "Invalid parse call result";

                        v.erase(v.begin() + i - 1, v.begin() + indexEnd);
                        v[i - 1] = tokenResult;
                        break;
                    }
                }

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

                DEBUG_LOG("Found nested (), invoking...");
                auto recursiveResult = ComputeValue(partVec);
                v.erase(v.begin() + i, v.begin() + result);
                v[i] = recursiveResult;

                break;
            }

            if (v.size() == tmp.size())
                break;
        }

        DEBUG_LOG("Parsing * /");
        // * / %
        while (true)
        {
            tmp = v;

            for (size_t i = 1; i + 1 < v.size(); i += 2)
            {
                auto token = v.at(i);
                if (!token->IsType(PTokenType::Sym))
                    throw "Unexpected token";

                auto sym = token->Cast<SymToken>()->Value;
                if (sym == "*" || sym == "/" || sym == "%")
                {
                    auto left = v.at(i - 1);
                    auto right = v.at(i + 1);

                    if (!ValidMDASToken(left) || !ValidMDASToken(right))
                        throw "Unexpected token type for MD";
                    
                    auto leftInt = GetMDASValue(left, m_Sys->GetRegistry());
                    auto rightInt = GetMDASValue(right, m_Sys->GetRegistry());

                    int result = 0;
                    if (sym == "*")
                    {
                        result = leftInt * rightInt;
                        DEBUG_LOG("Computing " << leftInt << " * " << rightInt << " = " << result);
                    }
                    else if (sym == "/")
                    {
                        result = leftInt / rightInt;
                        DEBUG_LOG("Computing " << leftInt << " / " << rightInt << " = " << result);
                    }
                    else
                    {
                        result = leftInt % rightInt;
                        DEBUG_LOG("Computing " << leftInt << " % " << rightInt << " = " << result);
                    }

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

        DEBUG_LOG("Parsing + -");
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

                    bool leftValidForText = (left->IsType(PTokenType::Text) || (left->IsType(PTokenType::Var) && m_Sys->GetRegistry()->HasEntry<std::string>(left->Cast<VarToken>()->Value)));
                    bool rightValidForText = (right->IsType(PTokenType::Text) || (right->IsType(PTokenType::Var) && m_Sys->GetRegistry()->HasEntry<std::string>(right->Cast<VarToken>()->Value)));
                    if (leftValidForText || rightValidForText)
                    {
                        DEBUG_LOG("Found Text");
                        if (sym != "+")
                            throw "Unexpected symbol, expected '+'";

                        std::stringstream sstr;
                        if (left->IsType(PTokenType::Text))
                        {
                            auto leftStr = left->Cast<TextToken>()->Value;
                            DEBUG_LOG("Concat Left Text(" << leftStr << ")");
                            sstr << leftStr;
                        }
                        else if (left->IsType(PTokenType::Int))
                        {
                            auto leftInt = left->Cast<NumberToken>()->Value;
                            DEBUG_LOG("Concat Left Int(" << leftInt << ")");
                            sstr << leftInt;
                        }
                        else if (left->IsType(PTokenType::Var))
                        {
                            auto name = left->Cast<VarToken>()->Value;
                            if (m_Sys->GetRegistry()->HasEntry<int>(name))
                            {
                                auto leftInt = m_Sys->GetRegistry()->GetEntry<int>(name);
                                DEBUG_LOG("Concat Left Var::Int(" << leftInt << ")");

                                sstr << leftInt;
                            }
                            else if (m_Sys->GetRegistry()->HasEntry<std::string>(name))
                            {
                                auto leftStr = m_Sys->GetRegistry()->GetEntry<std::string>(name);
                                DEBUG_LOG("Concat Left Var::Text(" << leftStr << ")");

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
                            DEBUG_LOG("Concat Right Text(" << rightStr << ")");
                            sstr << rightStr;
                        }
                        else if (right->IsType(PTokenType::Int))
                        {
                            auto rightInt = right->Cast<NumberToken>()->Value;
                            DEBUG_LOG("Concat Right Int(" << rightInt << ")");
                            sstr << rightInt;
                        }
                        else if (right->IsType(PTokenType::Var))
                        {
                            auto name = right->Cast<VarToken>()->Value;
                            if (m_Sys->GetRegistry()->HasEntry<int>(name))
                            {
                                auto rightInt = m_Sys->GetRegistry()->GetEntry<int>(name);
                                DEBUG_LOG("Concat Right Var::Int(" << rightInt << ")");

                                sstr << rightInt;
                            }
                            else if (m_Sys->GetRegistry()->HasEntry<std::string>(name))
                            {
                                auto rightStr = m_Sys->GetRegistry()->GetEntry<std::string>(name);
                                DEBUG_LOG("Concat Right Var::Text(" << rightStr << ")");

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
                    
                    auto leftInt = GetMDASValue(left, m_Sys->GetRegistry());
                    auto rightInt = GetMDASValue(right, m_Sys->GetRegistry());

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

        DEBUG_LOG("Parsing conditionals");
        // == != < > <= >=
        while (true)
        {
            tmp = v;

            for (size_t i = 1; i + 1 < v.size(); i += 2)
            {
                auto token = v.at(i);
                if (!token->IsType(PTokenType::Sym))
                    throw "Unexpected token";

                auto sym = token->Cast<SymToken>()->Value;
                if (sym == "==" || sym == "!=" || sym == "<" || sym == ">" || sym == "<=" || sym == ">=")
                {
                    auto left = v.at(i - 1);
                    auto right = v.at(i + 1);

                    bool isLeftText = (left->IsType(PTokenType::Text));
                    bool isRightText = (right->IsType(PTokenType::Text));

                    if (isLeftText != isRightText)
                        throw "Conditional type mismatch";

                    bool resultState = false;

                    if (isLeftText)
                    {
                        if (sym != "==" && sym != "!=")
                            throw "Unexpected symbol, expected '==' or '!='";
                        
                        auto leftStr = left->Cast<TextToken>()->Value;
                        auto rightStr = right->Cast<TextToken>()->Value;

                        DEBUG_LOG("Condition Text (" << leftStr << " " << sym << " " << rightStr << ")");

                        if (sym == "==") resultState = (leftStr == rightStr);
                        else resultState = (leftStr != rightStr);
                    }
                    else
                    {
                        auto leftInt = GetMDASValue(left, m_Sys->GetRegistry());
                        auto rightInt = GetMDASValue(right, m_Sys->GetRegistry());

                        DEBUG_LOG("Condition Number (" << leftInt << " " << sym << " " << rightInt << ")");

                        if (sym == "==") resultState = (leftInt == rightInt);
                        else if (sym == "!=") resultState = (leftInt != rightInt);
                        else if (sym == "<") resultState = (leftInt < rightInt);
                        else if (sym == ">") resultState = (leftInt > rightInt);
                        else if (sym == "<=") resultState = (leftInt <= rightInt);
                        else resultState = (leftInt >= rightInt);
                    }

                    auto ref = Ref<NumberToken>::Create(resultState ? 1 : 0);
                    v[i] = ref;
                    v.erase(v.begin() + i + 1);
                    v.erase(v.begin() + i - 1);
                    break;
                }
            }

            if (v.size() == tmp.size())
                break;
        }

        DEBUG_LOG("Parsing logical operators");
        // &&
        while (true)
        {
            tmp = v;

            for (size_t i = 1; i + 1 < v.size(); i += 2)
            {
                auto token = v.at(i);
                if (!token->IsType(PTokenType::Sym))
                    throw "Unexpected token";

                auto sym = token->Cast<SymToken>()->Value;
                if (sym == "&&" || sym == "||")
                {
                    auto left = v.at(i - 1);
                    auto right = v.at(i + 1);

                    bool isLeftText = (left->IsType(PTokenType::Text));
                    bool isRightText = (right->IsType(PTokenType::Text));

                    if (isLeftText || isRightText)
                        throw "Logical operation type mismatch";

                    bool resultState = false;

                    auto leftInt = GetMDASValue(left, m_Sys->GetRegistry());
                    auto rightInt = GetMDASValue(right, m_Sys->GetRegistry());

                    DEBUG_LOG("Logical (" << leftInt << " " << sym << " " << rightInt << ")");

                    if (sym == "&&") resultState = ((leftInt != 0) && (rightInt != 0));
                    else resultState = ((leftInt != 0) || (rightInt != 0));

                    auto ref = Ref<NumberToken>::Create(resultState ? 1 : 0);
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

        DEBUG_LOG("Exiting");
        return v[0];
        
    }
}