#include "tokenizer.h"
#include "strutil.h"

#include <regex>

namespace Dern
{
    struct TokenizerData
    {
        std::unordered_set<std::string> comparisonSet;
    };

    static TokenizerData* s_Data = nullptr;

    struct NamedRegex
    {
        std::regex Regex;
        std::string Name;

        NamedRegex(const std::string& rex, const std::string& name) : Regex(rex, std::regex_constants::ECMAScript), Name(name) {}

        operator std::regex() { return Regex; }
        bool operator==(const std::string& other) const { return Name == other; }
    };

    static NamedRegex CreateRegex(const std::string& name, std::initializer_list<std::string> l)
    {
        std::vector<std::string> v(l);
        std::stringstream sstr;
        sstr << "(";
        for (size_t index = 0; index < v.size(); index++)
        {
            if (index > 0) sstr << "|";

            std::string str = v[index];
            for (auto ch : str)
                sstr << "\\" << ch;
        }
        sstr << ")";

        return NamedRegex(sstr.str(), name);
    }

    Tokenizer::Tokenizer(const Scope<Code>& code)
        : m_Code(code) {}
    
    Tokenizer::~Tokenizer() {}

    static bool EqualsRegex(NamedRegex& rex, const std::string& str)
    {
        std::smatch sm;
        if (!std::regex_search(str, sm, rex.Regex)) return false;

        return !sm.prefix().matched && !sm.suffix().matched;
    }

    static bool StartsRegex(NamedRegex& rex, const std::string& str, int& len)
    {
        std::smatch sm;
        if (!std::regex_search(str, sm, rex.Regex)) return false;

        bool result = !sm.prefix().matched;
        if (result) len = sm[0].length();
        return result;
    }

    static int CreateWords(const std::string& str, std::vector<std::string>& words)
    {
        std::string syms = " \n\t\r\b\f\"";
        std::unordered_set<unsigned char> cmap;
        for (auto ch : syms)
        {
            cmap.insert(static_cast<unsigned char>(ch));
        }

        auto findSymsFn = [&cmap](unsigned char ch)
        {
            return cmap.find(ch) != cmap.end();
        };

        int count = 0;
        auto index = str.begin();
        auto lastIndex = index;
        while ((index = std::find_if(lastIndex, str.end(), findSymsFn)) != str.end())
        {
            words.push_back(std::string(lastIndex, index));
            if (*index == '"')
            {
                std::stringstream sstr;
                sstr << *(index++);
                while (index != str.end())
                {
                    if (*index == '\\')
                    {
                        sstr << *(index++) << *(index++);
                        continue;
                    }
                    else if (*index != '"')
                    {
                        sstr << *(index++);
                        continue;
                    }

                    sstr << *(index++);
                    break;
                }

                words.push_back(sstr.str());
                lastIndex = index;
                count += 2;
                continue;
            }

            lastIndex = index + 1;
            ++count;
        }

        words.push_back(std::string(lastIndex, str.end()));

        return count;
    }

    static void FixStringLiteral(std::string& literal)
    {
        std::stringstream sstr;
        for (size_t i = 0; i < literal.length(); i++)
        {
            if (literal[i] != '\\') sstr << literal[i];
            else
            {
                if ((++i) < literal.length())
                {
                    char c = literal[i];
                    if (c == 'n') sstr << "\n";
                    else if (c == 'r') sstr << "\r";
                    else if (c == 'f') sstr << "\f";
                    else if (c == 't') sstr << "\t";
                    else if (c == 'b') sstr << "\b";
                    else sstr << c;
                }
            }
        }
        literal = sstr.str();
    }

    bool Tokenizer::Tokenize()
    {
        NamedRegex wordRegex("[a-z][a-zA-Z0-9]*", "word");
        NamedRegex numRegex("\\-?[0-9]+", "num");
        NamedRegex symRegex = CreateRegex("sym",
        {
            // Triple
            "===", "<=>",
            // Double
            "!=", "==", "=>", "<=", ">=", "<<", ">>",
            "++", "--", "+=", "-=", "*=", "/=", "%=",
            "::", "->", "&&", "||",
            // Single
            "=", ";", "+", "-", "*", "/", "%", "(", ")", "[", "]", "{", "}",
            "<", ">", "!", ":", ","
        });

        if (s_Data == nullptr)
        {
            s_Data = new TokenizerData();

            std::unordered_set<std::string> comparisonSyms
            {
                "<=", "<", ">=", ">", "!=", "=="
            };

            s_Data->comparisonSet.merge(comparisonSyms);
        }

        NamedRegex strRegex("\"((\\\\.|.)*)\"", "str");

        NamedRegex typeRegex("\\$[a-z]+", "type");

        std::unordered_set<std::string> keywordSet({
            "print", "write",
            "var",
            "func", "return",
            "if", "unless", "else", "elif", "elun",
            "for", "while", "until",
            "incl",
            "true", "false"
        });

        NamedRegex allRegex[5] = {
            wordRegex, numRegex, symRegex, strRegex, typeRegex
        };

        std::vector<std::string> words;
        CreateWords(m_Code->Raw(), words);

        for (const auto& str : words)
        {
            if (EqualsRegex(wordRegex, str))
            {
                if (keywordSet.find(str) != keywordSet.end()) m_Tokens.emplace_back(TokenType::Keyword, str);
                else m_Tokens.emplace_back(TokenType::Name, str);
            }
            else if (EqualsRegex(numRegex, str))
            {
                m_Tokens.emplace_back(TokenType::Int, str);
            }
            else if (EqualsRegex(symRegex, str))
            {
                m_Tokens.emplace_back(TokenType::Sym, str);
            }
            else if (EqualsRegex(strRegex, str))
            {
                auto literal = str.substr(1, str.length() - 2);
                FixStringLiteral(literal);
                m_Tokens.emplace_back(TokenType::Text, literal);
            }
            else if (EqualsRegex(typeRegex, str))
            {
                auto type = str.substr(1);
                m_Tokens.emplace_back(TokenType::Type, type);
            }
            else
            {
                std::string tmp = str;
                while (tmp.length())
                {
                    int len = 0;
                    for (auto& rex : allRegex)
                    {
                        if (StartsRegex(rex, tmp, len))
                        {
                            std::string mstr = tmp.substr(0, len);
                            tmp = tmp.substr(len);

                            if (rex == "word")
                            {
                                if (keywordSet.find(mstr) != keywordSet.end()) m_Tokens.emplace_back(TokenType::Keyword, mstr);
                                else m_Tokens.emplace_back(TokenType::Name, mstr);
                            }
                            else if (rex == "num")
                            {
                                m_Tokens.emplace_back(TokenType::Int, mstr);
                            }
                            else if (rex == "sym")
                            {
                                m_Tokens.emplace_back(TokenType::Sym, mstr);
                            }
                            else if (rex == "str")
                            {
                                auto literal = mstr.substr(1, mstr.length() - 2);
                                FixStringLiteral(literal);
                                m_Tokens.emplace_back(TokenType::Text, literal);
                            }
                            else if (rex == "type")
                            {
                                auto type = mstr.substr(1);
                                m_Tokens.emplace_back(TokenType::Type, type);
                            }
                            else
                            {
                                std::cerr << "Failed to read nested token type (" << str << ", " << tmp << ", " << mstr << ")\n";
                                return false;
                            }

                            break;
                        }
                    }

                    if (len == 0)
                    {
                        std::cerr << "Failed to read nested token (" << str << ", " << tmp << ")\n";
                        return false;
                    }
                }
            }

        }

        return true;
    }

    bool Tokenizer::IsComparisonToken(const Ref<Token>& token)
    {
        if (!token->IsType(TokenType::Sym)) return false;

        return s_Data->comparisonSet.find(token->GetData()) != s_Data->comparisonSet.end();
    }
}