#include "pch.h"
#include "program.h"
#include "tokenizer.h"
#include "registry.h"
#include "typeparser.h"
#include "funcdef.h"
#include "paramdef.h"
#include "valuehelper.h"
#include "stackframe.h"

//#define ENABLE_DEBUG_READ
//#define ENABLE_DEBUG_RESULT
//#define ENABLE_CALL_LOG

#ifdef ENABLE_DEBUG_READ
    #define DEBUG_READ(token) std::cout << "Read[" << m_InstructionIndex << "]: " << *(token) << "\n"
#else
    #define DEBUG_READ(token)
#endif

#ifdef ENABLE_DEBUG_RESULT
    #define DEBUG_RESULT(data, type) std::cout << "Result[" << m_InstructionIndex << "]: " << type << "(" << data << ")\n";
#else
    #define DEBUG_RESULT(data, type)
#endif

#ifdef ENABLE_CALL_LOG
    #define CALL_LOG(stream) std::cout << "Call Log: " << stream << "\n";
#else
    #define CALL_LOG(stream)
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

    const Ref<Token>& LProgram::GetToken(ECount increment, int offset)
    {
        return m_Data->At(GetIndex(increment) + offset);
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

    static Scope<TokenData> LoadTokenData(const Scope<Code>& code)
    {
        auto tokenizer = CreateScope<Tokenizer>(code);
        if (!tokenizer->Tokenize())
        {
            std::cerr << "Failed to tokenize code!\n";
            return nullptr;
        }

        return tokenizer->GetTokenData();
    }

    void LProgram::Run()
    {
        try
        {
            m_Data = LoadTokenData(m_Code);
            if (!m_Data) return;

            m_Registry = Registry::Create();
            m_InstructionIndex = 0;

            RunImpl();
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

    Ref<StoredValue> LProgram::RunImpl()
    {
        while (m_InstructionIndex < m_Data->Count())
        {
            m_TokenStack[0] = GetToken();
            DEBUG_READ(m_TokenStack[0]);

            if (m_TokenStack[0]->IsType(TokenType::Keyword))
            {
                if (m_TokenStack[0]->IsValue("var"))
                {
                    m_TokenStack[1] = GetToken(ECount::Pre);
                    DEBUG_READ(m_TokenStack[1]);
                    if (!m_TokenStack[1]->IsType(TokenType::Name))
                    {
                        std::cerr << "Failed to var name\n";
                        return RNULL();
                    }

                    std::string varName = m_TokenStack[1]->GetData();

                    m_TokenStack[2] = GetToken(ECount::Pre);
                    
                    DEBUG_READ(m_TokenStack[2]);
                    if (!m_TokenStack[2]->IsType(TokenType::Sym) || !m_TokenStack[2]->IsValue("="))
                    {
                        std::cerr << "Expected =\n";
                        return RNULL();
                    }

                    GetIndex(ECount::Pre);
                    TypeParser parser(m_Registry, 1);
                    auto result = parser.ComputeValue([&](ParseMem& mem)
                    {
                        Ref<Token> token = GetToken(ECount::Post);
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
                else if (m_TokenStack[0]->IsValue("print"))
                {
                    m_TokenStack[1] = GetToken(ECount::Pre);
                    DEBUG_READ(m_TokenStack[1]);
                    if (!m_TokenStack[1]->IsType(TokenType::Sym) || !m_TokenStack[1]->IsValue("("))
                    {
                        std::cerr << "Unexpected '" << m_TokenStack[1]->GetData() << "', expected '('\n";
                        return RNULL();
                    }

                    GetIndex(ECount::Pre);
                    TypeParser parser(m_Registry, 1);
                    auto result = parser.ComputeValue([&](ParseMem& mem)
                    {
                        Ref<Token> token = GetToken(ECount::Post);
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

                    m_TokenStack[2] = GetToken(ECount::Post);
                    DEBUG_READ(m_TokenStack[2]);
                    if (!m_TokenStack[2]->IsType(TokenType::Sym) || !m_TokenStack[2]->IsValue(";"))
                    {
                        std::cerr << "Unexpected '" << m_TokenStack[2]->GetData() << "', expected ';'\n";
                        return RNULL();
                    }
                }
                else if (m_TokenStack[0]->IsValue("if"))
                {
                    m_TokenStack[1] = GetToken(ECount::Pre);
                    DEBUG_READ(m_TokenStack[1]);
                    if (!m_TokenStack[1]->IsType(TokenType::Sym) || !m_TokenStack[1]->IsValue("("))
                    {
                        std::cerr << "Unexpected '" << m_TokenStack[1]->GetData() << "', expected '('\n";
                        return RNULL();
                    }

                    GetIndex(ECount::Pre);
                    TypeParser leftParser(m_Registry, 1);
                    auto leftResult = leftParser.ComputeValue([&](ParseMem& mem)
                    {
                        Ref<Token> token = GetToken(ECount::Post);
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

                    m_TokenStack[2] = GetToken(ECount::None, -1);
                    DEBUG_READ(m_TokenStack[2]);
                    if (!Tokenizer::IsComparisonToken(m_TokenStack[2]))
                    {
                        std::cout << "Unexpected token '" << *m_TokenStack[2] << "'!\n";
                        return RNULL();
                    }

                    std::string op = m_TokenStack[2]->GetData();

                    TypeParser rightParser(m_Registry, 1);
                    auto rightResult = rightParser.ComputeValue([&](ParseMem& mem)
                    {
                        Ref<Token> token = GetToken(ECount::Post);
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
                        return RNULL();
                    }

                    bool isTrue = false;
                    if (leftResult->IsOfType<int>())
                    {
                        if (!rightResult->IsOfType<int>())
                        {
                            std::cerr << "Unexpected comparison type mismatch, expected right int!\n";
                            return RNULL();
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
                            return RNULL();
                        }

                        std::string leftStr = leftResult->GetData<std::string>();
                        std::string rightStr = rightResult->GetData<std::string>();

                        isTrue = ComparisonOperator<std::string>(leftStr, rightStr, op);
                    }
                    else
                    {
                        std::cerr << "Unexpected comparison type, expected left int or text!\n";
                        return RNULL();
                    }

                    if (isTrue)
                    {
                        m_TokenStack[3] = GetToken(ECount::Post);
                        DEBUG_READ(m_TokenStack[3]);
                        if (!m_TokenStack[3]->IsType(TokenType::Sym) || !m_TokenStack[3]->IsValue("{"))
                        {
                            std::cerr << "Unexpected '" << m_TokenStack[3]->GetData() << "'. Expected '{'!\n";
                            return RNULL();
                        }

                        m_Registry = Registry::Create(m_Registry);
                        auto frame = Ref<ScopeStackFrame>::Create(m_Registry);
                        m_FrameStack.push_back(frame);

                        continue;
                    }
                    else
                    {
                        m_TokenStack[3] = GetToken(ECount::Post);
                        DEBUG_READ(m_TokenStack[3]);
                        if (!m_TokenStack[3]->IsType(TokenType::Sym) || !m_TokenStack[3]->IsValue("{"))
                        {
                            std::cerr << "Unexpected '" << m_TokenStack[3]->GetData() << "'. Expected '{'!\n";
                            return RNULL();
                        }

                        int curl = 0;
                        do {
                            auto token = GetToken(ECount::Post);
                            DEBUG_READ(token);
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

                        m_TokenStack[4] = GetToken();
                        DEBUG_READ(m_TokenStack[4]);
                        if (!m_TokenStack[4]->IsType(TokenType::Keyword) || !m_TokenStack[4]->IsValue("else"))
                            continue;

                        // Found else

                        m_TokenStack[5] = GetToken(ECount::Pre);
                        DEBUG_READ(m_TokenStack[5]);
                        if (!m_TokenStack[5]->IsType(TokenType::Sym) || !m_TokenStack[5]->IsValue("{"))
                        {
                            std::cerr << "Unexpected '" << m_TokenStack[5]->GetData() << "', Expected '{'!\n";
                            return RNULL();
                        }

                        GetIndex(ECount::Pre);

                        m_Registry = Registry::Create(m_Registry);
                        auto frame = Ref<ScopeStackFrame>::Create(m_Registry);
                        m_FrameStack.push_back(frame);

                        continue;
                    }
                    
                    
                }
                else if (m_TokenStack[0]->IsValue("func"))
                {
                    m_TokenStack[1] = GetToken(ECount::Pre);
                    DEBUG_READ(m_TokenStack[1]);
                    if (!m_TokenStack[1]->IsType(TokenType::Name))
                    {
                        std::cerr << "Unexpected token type '" << *m_TokenStack[1] << "'. Expected TName!\n";
                        return RNULL();
                    }

                    std::string funcName = m_TokenStack[1]->GetData();

                    m_TokenStack[2] = GetToken(ECount::Pre);
                    DEBUG_READ(m_TokenStack[2]);
                    if (!m_TokenStack[2]->IsType(TokenType::Sym) || !m_TokenStack[2]->IsValue("("))
                    {
                        std::cerr << "Unexpected '" << m_TokenStack[2]->GetData() << "'. Expected '('!\n";
                        return RNULL();
                    }

                    GetIndex(ECount::Pre);
                    std::vector<ParamDef> params;
                    while (true) {
                        m_TokenStack[3] = GetToken(ECount::Post);
                        DEBUG_READ(m_TokenStack[3]);
                        if (m_TokenStack[3]->IsType(TokenType::Sym) && m_TokenStack[3]->IsValue(")"))
                            break;

                        if (!m_TokenStack[3]->IsType(TokenType::Type))
                        {
                            std::cerr << "Unexpected token type '" << *m_TokenStack[3] << "'. Expected TType!\n";
                            return RNULL();
                        }

                        EType type = FromStringToType(m_TokenStack[3]->GetData());
                        
                        m_TokenStack[4] = GetToken(ECount::Post);
                        DEBUG_READ(m_TokenStack[4]);
                        if (!m_TokenStack[4]->IsType(TokenType::Sym) || !m_TokenStack[4]->IsValue(":"))
                        {
                            std::cerr << "Unexpected token '" << m_TokenStack[4]->GetData() << "'. Expected ':'!\n";
                            return RNULL();
                        }

                        m_TokenStack[5] = GetToken(ECount::Post);
                        DEBUG_READ(m_TokenStack[5]);
                        if (!m_TokenStack[5]->IsType(TokenType::Name))
                        {
                            std::cerr << "Unexpected token type '" << *m_TokenStack[5] << "'. Expected TName!\n";
                            return RNULL();
                        }

                        std::string name = m_TokenStack[5]->GetData();

                        params.emplace_back(type, name);
                        m_TokenStack[3] = GetToken(ECount::Post);
                        DEBUG_READ(m_TokenStack[3]);
                        if (!m_TokenStack[3]->IsType(TokenType::Sym))
                        {
                            std::cerr << "Unexpected token type '" << *m_TokenStack[3] << "'. Expected TSym!\n";
                            return RNULL();
                        }

                        if (!m_TokenStack[3]->IsValue(",") && !m_TokenStack[3]->IsValue(")"))
                        {
                            std::cerr << "Unexpected token '" << m_TokenStack[3]->GetData() << "'. Expected ',' or ')'!\n";
                            return RNULL();
                        }

                        if (m_TokenStack[3]->IsValue(")"))
                            break;
                    }

                    m_TokenStack[6] = GetToken(ECount::Post);
                    DEBUG_READ(m_TokenStack[6]);
                    if (!m_TokenStack[6]->IsType(TokenType::Sym) || !m_TokenStack[6]->IsValue("->"))
                    {
                        std::cerr << "Unexpected token '" << m_TokenStack[6]->GetData() << "'. Expected '->'!\n";
                        return RNULL();
                    }

                    m_TokenStack[7] = GetToken(ECount::Post);
                    DEBUG_READ(m_TokenStack[7]);
                    if (!m_TokenStack[7]->IsType(TokenType::Type))
                    {
                        std::cerr << "Unexpected token type '" << *m_TokenStack[7] << "'. Expected TType!\n";
                        return RNULL();
                    }

                    EType returnType = FromStringToType(m_TokenStack[7]->GetData());
                    m_TokenStack[8] = GetToken(ECount::Post);
                    DEBUG_READ(m_TokenStack[8]);
                    if (!m_TokenStack[8]->IsType(TokenType::Sym) || !m_TokenStack[8]->IsValue("{"))
                    {
                        std::cerr << "Unexpected token '" << m_TokenStack[8]->GetData() << "'. Expected '{'!\n";
                        return RNULL();
                    }

                    int startPos = GetIndex();

                    int curl = 0;
                    while (true)
                    {
                        auto token = GetToken(ECount::Post);
                        DEBUG_READ(token);
                        if (token->IsType(TokenType::Sym) && token->IsValue("{"))
                        {
                            ++curl;
                        }
                        else if (token->IsType(TokenType::Sym) && token->IsValue("}"))
                        {
                            if (curl <= 0) break;
                            --curl;
                        }
                    }

                    int endPos = GetIndex() - 1;

                    auto funcDef = Ref<FuncDef>::Create(funcName, returnType, params, startPos, endPos);
                    if (m_FuncMap.find(funcName) != m_FuncMap.end())
                    {
                        std::cerr << "Unexpected func redefinition '" << funcName << "'!\n";
                        return RNULL();
                    }

                    m_FuncMap[funcName] = funcDef;
                    continue;
                }
                else
                {
                    std::cerr << "Unexpected keyword '" << m_TokenStack[0]->GetData() << "'!\n";
                    return RNULL();
                }
            }
            else if (m_TokenStack[0]->IsType(TokenType::Name))
            {
                std::string varName = m_TokenStack[0]->GetData();

                m_TokenStack[1] = GetToken(ECount::Pre);

                DEBUG_READ(m_TokenStack[1]);
                if (!m_TokenStack[1]->IsType(TokenType::Sym))
                {
                    std::cerr << "Unexpected token '" << *m_TokenStack[1] << "', expected symbol!\n";
                    return RNULL();
                }

                if (m_TokenStack[1]->IsValue("="))
                {
                    GetIndex(ECount::Pre);
                    TypeParser parser(m_Registry, 1);
                    auto result = parser.ComputeValue([&](ParseMem& mem)
                    {
                        Ref<Token> token = GetToken(ECount::Post);
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
                            m_Registry->SetEntry<int>(varName, data, false);
                        }
                        else if (result->IsOfType<std::string>())
                        {
                            auto data = result->GetData<std::string>();
                            DEBUG_RESULT(data, "string");
                            m_Registry->SetEntry<std::string>(varName, data, false);
                        }
                    }
                    else
                    {
                        DEBUG_RESULT("", "None");
                    }

                    continue;
                }
                else if (m_TokenStack[1]->IsValue("<=>"))
                {
                    m_TokenStack[2] = GetToken(ECount::Pre);

                    DEBUG_READ(m_TokenStack[2]);
                    if (!m_TokenStack[2]->IsType(TokenType::Name))
                    {
                        std::cerr << "Unexpected token '" << *m_TokenStack[2] << "'!\n";
                        return RNULL();
                    }

                    std::string varNameRight = m_TokenStack[2]->GetData();
                    m_TokenStack[3] = GetToken(ECount::Pre);
                    DEBUG_READ(m_TokenStack[3]);
                    if (!m_TokenStack[3]->IsType(TokenType::Sym) || !m_TokenStack[3]->IsValue(";"))
                    {
                        std::cerr << "Unexpected token '" << *m_TokenStack[3] << "' expected ';'!\n";
                        return RNULL();
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

                    if (undefinedCheck > 0) return RNULL();

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

                    GetIndex(ECount::Pre);
                    continue;
                }
                else if (m_TokenStack[1]->IsValue("("))
                {
                    if (m_FuncMap.find(varName) == m_FuncMap.end())
                    {
                        std::cerr << "Unexpected function name '" << varName << "'!\n";
                        return RNULL();
                    }

                    auto func = m_FuncMap.at(varName);

                    Ref<Registry> funcLocal = Registry::Create();
                    GetIndex(ECount::Pre);
                    std::vector<Ref<StoredValue>> params;
                    while (true) {
                        TypeParser parser(m_Registry, 1);
                        auto paramResult = parser.ComputeValue([&](ParseMem& mem)
                        {
                            Ref<Token> token = GetToken(ECount::Post);
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
                            else if (token->IsType(TokenType::Sym) && token->IsValue(","))
                            {
                                if (mem[0] <= 0) return Ref<Token>();
                            }

                            return token;
                        });

                        params.push_back(paramResult);
                        CALL_LOG("Param " << (params.size() - 1) << ": " << paramResult->ToString());

                        m_TokenStack[2] = GetToken(ECount::None, -1);
                        DEBUG_READ(m_TokenStack[2]);
                        if (m_TokenStack[2]->IsType(TokenType::Sym) && m_TokenStack[2]->IsValue(")"))
                            break;
                    }

                    CALL_LOG("Checking Size");
                    if (params.size() != (size_t)func->GetParamCount())
                    {
                        std::cerr << "Invalid func call argument count. Expected '" << func->GetParamCount() << "' but got '" << params.size() << "'!\n";
                        return RNULL();
                    }

                    for (size_t index = 0; index < params.size(); index++)
                    {
                        auto sv = params.at(index);
                        if (sv->IsOfType<int>())
                        {
                            if (func->At(index).GetType() != EType::Int)
                            {
                                std::cerr << "Unexpected type for '" << varName << "' argument #" << index << ". Expected " << FromTypeToString(func->At(index).GetType()) << ", got number!\n";
                                return RNULL();
                            }

                            auto name = func->At(index).GetName();
                            auto data = sv->GetData<int>();

                            CALL_LOG("Setting " << name << "=>Int(" << data << ")");
                            funcLocal->SetEntry<int>(name, data);
                        }
                        else if (sv->IsOfType<std::string>())
                        {
                            if (func->At(index).GetType() != EType::Text)
                            {
                                std::cerr << "Unexpected type for '" << varName << "' argument #" << index << ". Expected " << FromTypeToString(func->At(index).GetType()) << ", got text!\n";
                                return RNULL();
                            }

                            auto name = func->At(index).GetName();
                            auto data = sv->GetData<std::string>();

                            CALL_LOG("Setting " << name << "=>Text(" << data << ")");
                            funcLocal->SetEntry<std::string>(name, data);
                        }
                        else
                        {
                            std::cerr << "Unexpected data type for '" << varName << "' argument #" << index << "!\n";
                            return RNULL();
                        }
                    }

                    m_TokenStack[3] = GetToken(ECount::Post);
                    DEBUG_READ(m_TokenStack[3]);
                    if (!m_TokenStack[3]->IsType(TokenType::Sym) || !m_TokenStack[3]->IsValue(";"))
                    {
                        std::cerr << "Unexpected token '" << m_TokenStack[3]->GetData() << "'. Expected ';'!\n";
                        return RNULL();
                    }

                    auto call = Ref<FuncCall>::Create(GetIndex(), func, m_Registry, funcLocal);
                    auto frame = Ref<CallStackFrame>::Create(funcLocal, call);
                    m_FrameStack.push_back(frame);
                    m_InstructionIndex = func->GetStart();
                    m_Registry = funcLocal;

                    CALL_LOG(call->ToString() << " " << m_InstructionIndex);

                    RunImpl();
                    continue;
                }
                else
                {
                    std::cerr << "Unexpected sym token '" << *m_TokenStack[1] << "'!\n";
                    return RNULL();
                }
            }
            else if (m_TokenStack[0]->IsType(TokenType::Sym))
            {
                if (m_TokenStack[0]->IsValue("}"))
                {
                    // Poll EOF
                    m_Data->PollEOF(GetIndex() + 1);

                    auto frame = m_FrameStack[m_FrameStack.size() - 1];
                    m_FrameStack.erase(m_FrameStack.begin() + (m_FrameStack.size() - 1));
                    if (frame->GetType() == StackFrameType::Call)
                    {
                        auto funcCall = frame.Cast<CallStackFrame>()->GetFuncCall();
                        m_InstructionIndex = funcCall->GetReturnIndex();
                        m_Registry = funcCall->GetParentRegistry();
                        return RNULL();
                    }
                    else
                    {
                        auto parentFrame = m_FrameStack[m_FrameStack.size() - 1];
                        m_Registry = parentFrame->GetRegistry();
                    }

                    m_TokenStack[1] = GetToken(ECount::Pre);
                    DEBUG_READ(m_TokenStack[1]);
                    if (!m_TokenStack[1]->IsType(TokenType::Keyword) || !m_TokenStack[1]->IsValue("else"))
                        continue;

                    m_TokenStack[2] = GetToken(ECount::Pre);
                    DEBUG_READ(m_TokenStack[2]);
                    if (!m_TokenStack[2]->IsType(TokenType::Sym) || !m_TokenStack[2]->IsValue("{"))
                    {
                        std::cerr << "Unexpected '" << m_TokenStack[2]->GetData() << "', expected '{'!\n";
                        return RNULL();
                    }

                    GetIndex(ECount::Pre);
                    int curl = 0;
                    do {
                        auto token = GetToken(ECount::Post);
                        DEBUG_READ(token);
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
                    std::cerr << "Unexpected token '" << *m_TokenStack[0] << "'!\n";
                    return RNULL();
                }
            }
            else
            {
                std::cerr << "Unexpected token '" << *m_TokenStack[0] << "'!\n";
                return RNULL();
            }
        }

        return RNULL();
    }
}

