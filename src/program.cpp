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

    template<typename T>
    static bool ComparisonOperator(const T& left, const T& right, const std::string& op)
    {
        if (op == "==") return left == right;
        else if (op == "!=") return left != right;
        else if (op == "<") return left < right;
        else if (op == ">") return left > right;
        else if (op == "<=") return left <= right;
        else if (op == ">=") return left >= right;
        
        return false;
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

            m_Registry = Registry::Create();
            int index = 0;
            Ref<Token> tokens[32];
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
                        TypeParser parser(m_Registry, 1);
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
                                m_Registry->SetEntry<int>(varName, data);
                            }
                            else if (result->IsOfType<std::string>())
                            {
                                auto data = result->GetData<std::string>();
                                DEBUG_RESULT(data, "string");
                                m_Registry->SetEntry<std::string>(varName, data);
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
                        TypeParser parser(m_Registry, 1);
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
                    else if (tokens[0]->IsValue("if"))
                    {
                        tokens[1] = data->At(++index);
                        DEBUG_READ(tokens[1]);
                        if (!tokens[1]->IsType(TokenType::Sym) || !tokens[1]->IsValue("("))
                        {
                            std::cerr << "Unexpected '" << tokens[1]->GetData() << "', expected '('\n";
                            return;
                        }

                        ++index;
                        TypeParser leftParser(m_Registry, 1);
                        auto leftResult = leftParser.ComputeValue([&](ParseMem& mem)
                        {
                            Ref<Token> token = data->At(index++);
                            DEBUG_READ(token);
                            if (token->IsType(TokenType::Sym) && token->IsValue("("))
                            {
                                ++mem[0];
                            }
                            else if (token->IsType(TokenType::Sym) && token->IsValue(")"))
                            {
                                if (mem[0] <= 0) throw "Unexpected ')'";
                                --mem[0];
                            }
                            else if (Tokenizer::IsComparisonToken(token))
                            {
                                if (mem[0] <= 0) return Ref<Token>();
                            }

                            return token;
                        });

                        tokens[2] = data->At(index - 1);
                        DEBUG_READ(tokens[2]);
                        if (!Tokenizer::IsComparisonToken(tokens[2]))
                        {
                            std::cout << "Unexpected token '" << *tokens[2] << "'!\n";
                            return;
                        }

                        std::string op = tokens[2]->GetData();

                        TypeParser rightParser(m_Registry, 1);
                        auto rightResult = rightParser.ComputeValue([&](ParseMem& mem)
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

                        if (!leftResult->HasData() || !rightResult->HasData())
                        {
                            std::cerr << "Unexpected, comparison expression expected!\n";
                            return;
                        }

                        bool isTrue = false;
                        if (leftResult->IsOfType<int>())
                        {
                            if (!rightResult->IsOfType<int>())
                            {
                                std::cerr << "Unexpected comparison type mismatch, expected right int!\n";
                                return;
                            }

                            int leftInt = leftResult->GetData<int>();
                            int rightInt = rightResult->GetData<int>();

                            isTrue = ComparisonOperator<int>(leftInt, rightInt, op);
                        }
                        else if (leftResult->IsOfType<std::string>())
                        {
                            if (!rightResult->IsOfType<std::string>())
                            {
                                std::cerr << "Unexpected comparison type mismatch, expected right text!\n";
                                return;
                            }

                            std::string leftStr = leftResult->GetData<std::string>();
                            std::string rightStr = rightResult->GetData<std::string>();

                            isTrue = ComparisonOperator<std::string>(leftStr, rightStr, op);
                        }
                        else
                        {
                            std::cerr << "Unexpected comparison type, expected left int or text!\n";
                            return;
                        }

                        if (isTrue)
                        {
                            tokens[3] = data->At(index++);
                            DEBUG_READ(tokens[3]);
                            if (!tokens[3]->IsType(TokenType::Sym) || !tokens[3]->IsValue("{"))
                            {
                                std::cerr << "Unexpected '" << tokens[3]->GetData() << "'. Expected '{'!\n";
                                return;
                            }

                            m_Registry = Registry::Create(m_Registry);

                            continue;
                        }
                        else
                        {
                            tokens[3] = data->At(index++);
                            DEBUG_READ(tokens[3]);
                            if (!tokens[3]->IsType(TokenType::Sym) || !tokens[3]->IsValue("{"))
                            {
                                std::cerr << "Unexpected '" << tokens[3]->GetData() << "'. Expected '{'!\n";
                                return;
                            }

                            int curl = 0;
                            do {
                                auto token = data->At(index++);
                                if (token->IsType(TokenType::Sym) && token->IsValue("{"))
                                {
                                    ++curl;
                                }
                                else if (token->IsType(TokenType::Sym) && token->IsValue("}"))
                                {
                                    if (curl <= 0) break;
                                    --curl;
                                }
                            } while (true);

                            tokens[4] = data->At(index);
                            if (!tokens[4]->IsType(TokenType::Keyword) || !tokens[4]->IsValue("else"))
                                continue;

                            // Found else

                            tokens[5] = data->At(++index);
                            if (!tokens[5]->IsType(TokenType::Sym) || !tokens[5]->IsValue("{"))
                            {
                                std::cerr << "Unexpected '" << tokens[5]->GetData() << "', Expected '{'!\n";
                                return;
                            }

                            ++index;

                            m_Registry = Registry::Create(m_Registry);

                            continue;
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
                        TypeParser parser(m_Registry, 1);
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
                                m_Registry->SetEntry<int>(varName, data);
                            }
                            else if (result->IsOfType<std::string>())
                            {
                                auto data = result->GetData<std::string>();
                                DEBUG_RESULT(data, "string");
                                m_Registry->SetEntry<std::string>(varName, data);
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
                        if (!m_Registry->HasAnyEntry(varName))
                        {
                            undefinedCheck++;
                            std::cerr << "Variable '" << varName << "' is undefined!\n";
                        }

                        if (!m_Registry->HasAnyEntry(varNameRight))
                        {
                            undefinedCheck++;
                            std::cerr << "Variable '" << varNameRight << "' is undefined!\n";
                        }

                        if (undefinedCheck > 0) return;

                        Ref<StoredValue> tmp = Ref<StoredValue>::Create();
                        if (m_Registry->HasEntry<int>(varName))
                        {
                            tmp->SetData<int>(m_Registry->GetEntry<int>(varName));
                        }
                        else if (m_Registry->HasEntry<std::string>(varName))
                        {
                            tmp->SetData<std::string>(m_Registry->GetEntry<std::string>(varName));
                        }

                        if (m_Registry->HasEntry<int>(varNameRight))
                        {
                            m_Registry->SetEntry<int>(varName, m_Registry->GetEntry<int>(varNameRight));
                        }
                        else if (m_Registry->HasEntry<std::string>(varNameRight))
                        {
                            m_Registry->SetEntry<std::string>(varName, m_Registry->GetEntry<std::string>(varNameRight));
                        }

                        if (tmp->IsOfType<int>())
                        {
                            m_Registry->SetEntry<int>(varNameRight, tmp->GetData<int>());
                        }
                        else if (tmp->IsOfType<std::string>())
                        {
                            m_Registry->SetEntry<std::string>(varNameRight, tmp->GetData<std::string>());
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
                else if (tokens[0]->IsType(TokenType::Sym))
                {
                    if (tokens[0]->IsValue("}"))
                    {
                        // Poll EOF
                        data->PollEOF(index + 1);

                        auto parent = m_Registry->GetParent();
                        if (parent)
                        {
                            m_Registry = parent;
                        }

                        tokens[1] = data->At(++index);
                        if (!tokens[1]->IsType(TokenType::Keyword) || !tokens[1]->IsValue("else"))
                            continue;

                        tokens[2] = data->At(++index);
                        if (!tokens[2]->IsType(TokenType::Sym) || !tokens[2]->IsValue("{"))
                        {
                            std::cerr << "Unexpected '" << tokens[2]->GetData() << "', expected '{'!\n";
                            return;
                        }

                        ++index;
                        int curl = 0;
                        do {
                            auto token = data->At(index++);
                            if (token->IsType(TokenType::Sym) && token->IsValue("{"))
                            {
                                ++curl;
                            }
                            else if (token->IsType(TokenType::Sym) && token->IsValue("}"))
                            {
                                if (curl <= 0) break;
                                --curl;
                            }
                        } while (true);

                        continue;
                    }
                    else
                    {
                        std::cerr << "Unexpected token '" << *tokens[0] << "'!\n";
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
        catch (const TokenDataEOF& eof)
        {
            if (eof.IsGood())
                return;

            std::cerr << "EOF Error: " << eof.GetIndex() << " >= " << eof.GetCount() << "!\n";
        }
    }
}

