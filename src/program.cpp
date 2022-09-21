#include "pch.h"
#include "program.h"
#include "tokenizer.h"
#include "registry.h"
#include "typeparser.h"

//#define ENABLE_DEBUG_READ
//#define ENABLE_DEBUG_RESULT

#ifdef ENABLE_DEBUG_READ
    #define DEBUG_READ(token) std::cout << "Read[" << index << "]: " << *(token) << "\n"
#else
    #define DEBUG_READ(token)
#endif

#ifdef ENABLE_DEBUG_RESULT
    #define DEBUG_RESULT(data, type) std::cout << "Result: " << type << "(" << data << ")\n";
#else
    #define DEBUG_RESULT(data, type)
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
        try
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

                if (tokens[0]->IsType(TokenType::Keyword))
                {
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
                        TypeParser parser(reg, 1);
                        auto result = parser.ComputeValue([&](ParseMem& mem)
                        {
                            Ref<Token> token = data->At(index++);
                            DEBUG_READ(token);
                            if (token->IsType(TokenType::Sym) && token->IsValue("("))
                            {
                                ++mem[0];
                            }
                            else if (token->IsType(TokenType::Sym) && token->IsValue(")"))
                            {
                                if (mem[0] <= 0) return Ref<Token>();
                                --mem[0];
                            }
                            else if (token->IsType(TokenType::Sym) && token->IsValue(";"))
                            {
                                if (mem[0] <= 0) return Ref<Token>();
                                throw "Unexpected ';'";
                            }
                            return token;
                        });

                        if (result->HasData())
                        {
                            if (result->IsOfType<int>())
                            {
                                auto data = result->GetData<int>();
                                DEBUG_RESULT(data, "int");
                                reg.SetEntry<int>(varName, data);
                            }
                            else if (result->IsOfType<std::string>())
                            {
                                auto data = result->GetData<std::string>();
                                DEBUG_RESULT(data, "string");
                                reg.SetEntry<std::string>(varName, data);
                            }
                        }
                        else
                        {
                            DEBUG_RESULT("", "None")
                        }

                        continue;
                    }
                    else if (tokens[0]->IsValue("print"))
                    {
                        tokens[1] = data->At(++index);
                        DEBUG_READ(tokens[1]);
                        if (!tokens[1]->IsType(TokenType::Sym) || !tokens[1]->IsValue("("))
                        {
                            std::cerr << "Unexpected '" << tokens[1]->GetData() << "', expected '('\n";
                            return;
                        }

                        ++index;
                        TypeParser parser(reg, 1);
                        auto result = parser.ComputeValue([&](ParseMem& mem)
                        {
                            Ref<Token> token = data->At(index++);
                            DEBUG_READ(token);
                            if (token->IsType(TokenType::Sym) && token->IsValue("("))
                            {
                                ++mem[0];
                            }
                            else if (token->IsType(TokenType::Sym) && token->IsValue(")"))
                            {
                                if (mem[0] <= 0) return Ref<Token>();
                                --mem[0];
                            }
                            else if (token->IsType(TokenType::Sym) && token->IsValue(";"))
                            {
                                throw "Unexpected ';'";
                            }
                            return token;
                        });

                        if (result->HasData())
                        {
                            if (result->IsOfType<int>())
                            {
                                auto data = result->GetData<int>();
                                DEBUG_RESULT(data, "int");
                                std::cout << data << "\n";
                            }
                            else if (result->IsOfType<std::string>())
                            {
                                auto data = result->GetData<std::string>();
                                DEBUG_RESULT(data, "string");
                                std::cout << data << "\n";
                            }
                            else
                            {
                                std::cout << "\n";
                            }
                        }
                        else
                        {
                            DEBUG_RESULT("", "None");
                            std::cout << "\n";
                        }

                        tokens[2] = data->At(index++);
                        if (!tokens[2]->IsType(TokenType::Sym) || !tokens[2]->IsValue(";"))
                        {
                            std::cerr << "Unexpected '" << tokens[2]->GetData() << "', expected ';'\n";
                            return;
                        }
                    }
                    else
                    {
                        std::cerr << "Unexpected keyword '" << tokens[0]->GetData() << "'!\n";
                        return;
                    }
                }
                else if (tokens[0]->IsType(TokenType::Name))
                {
                    std::string varName = tokens[0]->GetData();

                    tokens[1] = data->At(++index);

                    DEBUG_READ(tokens[1]);
                    if (!tokens[1]->IsType(TokenType::Sym))
                    {
                        std::cerr << "Unexpected token '" << *tokens[1] << "', expected symbol!\n";
                        return;
                    }

                    if (tokens[1]->IsValue("="))
                    {
                        ++index;
                        TypeParser parser(reg, 1);
                        auto result = parser.ComputeValue([&](ParseMem& mem)
                        {
                            Ref<Token> token = data->At(index++);
                            DEBUG_READ(token);
                            if (token->IsType(TokenType::Sym) && token->IsValue("("))
                            {
                                ++mem[0];
                            }
                            else if (token->IsType(TokenType::Sym) && token->IsValue(")"))
                            {
                                if (mem[0] <= 0) return Ref<Token>();
                                --mem[0];
                            }
                            else if (token->IsType(TokenType::Sym) && token->IsValue(";"))
                            {
                                if (mem[0] <= 0) return Ref<Token>();
                                throw "Unexpected ';'";
                            }
                            return token;
                        });

                        if (result->HasData())
                        {
                            if (result->IsOfType<int>())
                            {
                                auto data = result->GetData<int>();
                                DEBUG_RESULT(data, "int");
                                reg.SetEntry<int>(varName, data);
                            }
                            else if (result->IsOfType<std::string>())
                            {
                                auto data = result->GetData<std::string>();
                                DEBUG_RESULT(data, "string");
                                reg.SetEntry<std::string>(varName, data);
                            }
                        }
                        else
                        {
                            DEBUG_RESULT("", "None");
                        }

                        continue;
                    }
                    else if (tokens[1]->IsValue("<=>"))
                    {
                        tokens[2] = data->At(++index);

                        DEBUG_READ(tokens[2]);
                        if (!tokens[2]->IsType(TokenType::Name))
                        {
                            std::cerr << "Unexpected token '" << *tokens[2] << "'!\n";
                            return;
                        }

                        std::string varNameRight = tokens[2]->GetData();
                        tokens[3] = data->At(++index);
                        DEBUG_READ(tokens[3]);
                        if (!tokens[3]->IsType(TokenType::Sym) || !tokens[3]->IsValue(";"))
                        {
                            std::cerr << "Unexpected token '" << *tokens[3] << "' expected ';'!\n";
                            return;
                        }

                        int undefinedCheck = 0;
                        if (!reg.HasAnyEntry(varName))
                        {
                            undefinedCheck++;
                            std::cerr << "Variable '" << varName << "' is undefined!\n";
                        }

                        if (!reg.HasAnyEntry(varNameRight))
                        {
                            undefinedCheck++;
                            std::cerr << "Variable '" << varNameRight << "' is undefined!\n";
                        }

                        if (undefinedCheck > 0) return;

                        Ref<StoredValue> tmp = CreateRef<StoredValue>();
                        if (reg.HasEntry<int>(varName))
                        {
                            tmp->SetData<int>(reg.GetEntry<int>(varName));
                        }
                        else if (reg.HasEntry<std::string>(varName))
                        {
                            tmp->SetData<std::string>(reg.GetEntry<std::string>(varName));
                        }

                        if (reg.HasEntry<int>(varNameRight))
                        {
                            reg.SetEntry<int>(varName, reg.GetEntry<int>(varNameRight));
                        }
                        else if (reg.HasEntry<std::string>(varNameRight))
                        {
                            reg.SetEntry<std::string>(varName, reg.GetEntry<std::string>(varNameRight));
                        }

                        if (tmp->IsOfType<int>())
                        {
                            reg.SetEntry<int>(varNameRight, tmp->GetData<int>());
                        }
                        else if (tmp->IsOfType<std::string>())
                        {
                            reg.SetEntry<std::string>(varNameRight, tmp->GetData<std::string>());
                        }

                        ++index;
                        continue;
                    }
                    else
                    {
                        std::cerr << "Unexpected sym token '" << *tokens[1] << "'!\n";
                        return;
                    }
                }
                else
                {
                    std::cerr << "Unexpected token '" << *tokens[0] << "'!\n";
                    return;
                }
            }
        }
        catch (const char* message)
        {
            std::cerr << "Error: " << message << "\n";
        }
    }
}

