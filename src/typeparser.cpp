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

    Ref<ParseToken> TypeParser::ComputeValue(std::vector<Ref<ParseToken>> v)
    {
        std::vector<Ref<ParseToken>> tmp;

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

                    if (!left->IsType(PTokenType::Int) || !right->IsType(PTokenType::Int))
                        throw "Unexpected token type for MD";
                    
                    auto leftInt = left->Cast<NumberToken>()->Value;
                    auto rightInt = right->Cast<NumberToken>()->Value;

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

                    if (!left->IsType(PTokenType::Int) || !right->IsType(PTokenType::Int))
                        throw "Unexpected token type for AS";
                    
                    auto leftInt = left->Cast<NumberToken>()->Value;
                    auto rightInt = right->Cast<NumberToken>()->Value;

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