#include "pch.h"
#include "program.h"
#include "tokenizer.h"
#include "registry.h"
#include "typeparser.h"

//#define ENABLE_DEBUG_READ

#ifdef ENABLE_DEBUG_READ
    #define DEBUG_READ(token) std::cout << "Read[" << index << "]: " << *(token) << "\n"
#else
    #define DEBUG_READ(token)
#endif

namespace Dern
{
	LProgram::LProgram(const std::string& filePath)
    {
        m_Code = CreateScope<Code>(filePath);
    }

    LProgram::~LProgram()
    {
    }

    void LProgram::Run()
    {
        auto tokenizer = CreateScope<Tokenizer>(m_Code);
        if (!tokenizer->Tokenize())
        {
            std::cerr << "Failed to tokenize code!\n";
            return;
        }

        Scope<TokenData> data = tokenizer->GetTokenData();

        Registry reg;
        int index = 0;
        Ref<Token> tokens[5];
        while (index < data->Count())
        {
            tokens[0] = data->At(index);
            DEBUG_READ(tokens[0]);
            if (!tokens[0]->IsType(TokenType::Keyword))
            {
                std::cerr << "Failed to read first keyword\n";
                return;
            }

            if (tokens[0]->IsValue("var"))
            {
                tokens[1] = data->At(++index);
                DEBUG_READ(tokens[1]);
                if (!tokens[1]->IsType(TokenType::Name))
                {
                    std::cerr << "Failed to var name\n";
                    return;
                }

                std::string varName = tokens[1]->GetData();

                tokens[2] = data->At(++index);
                
                DEBUG_READ(tokens[2]);
                if (!tokens[2]->IsType(TokenType::Sym) || !tokens[2]->IsValue("="))
                {
                    std::cerr << "Expected =\n";
                    return;
                }

                ++index;
                auto result = TypeParser::ComputeValue([&]()
                {
                    Ref<Token> token = data->At(index++);
                    DEBUG_READ(token);
                    if (*token == TokenType::Sym && (*token == ";" || *token == ")")) return Ref<Token>();
                    return token;
                });

                if (result->HasData())
                {
                    if (result->IsOfType<int>())
                    {
                        reg.SetEntry<int>(varName, result->GetData<int>());
                    }
                    else if (result->IsOfType<std::string>())
                    {
                        reg.SetEntry<std::string>(varName, result->GetData<std::string>());
                    }
                }

                continue;
            }
            else
            {
                break;
            }

            tokens[1] = data->At(++index);

            auto result = TypeParser::ComputeValue([&]()
            {
                Ref<Token> token = data->At(index++);
                DEBUG_READ(token);
                if (*token == TokenType::Sym && (*token == ";" || *token == ")")) return Ref<Token>();
                return token;
            });
        }
    }
}

