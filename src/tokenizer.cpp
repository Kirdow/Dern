#include "pch.h"
#include "tokenizer.h"
#include "strutil.h"

#include <regex>

namespace Dern
{
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

    bool Tokenizer::Tokenize()
    {
        NamedRegex wordRegex("[a-z][a-z0-9]*", "word");
        NamedRegex numRegex("[0-9]+", "num");
        NamedRegex symRegex = CreateRegex("sym",
        {
            // Triple
            "===", "<=>",
            // Double
            "!=", "==", "=>", "<=", ">=", "<<", ">>",
            "++", "--", "+=", "-=", "*=", "/=", "%=",
            // Single
            "=", ";", "+", "-", "*", "/", "(", ")", "[", "]", "{", "}"
        });
        //NamedRegex symRegex("(\\=|\\;|\\+|\\-|\\*|\\/|\\(|\\))", "sym");

        NamedRegex allRegex[3] = {
            wordRegex, numRegex, symRegex
        };

        std::vector<std::string> words;
        StrUtil::SplitSymbols(m_Code->Raw(), " \n\t\r\b\f", words);
        for (const auto& str : words)
        {
            if (EqualsRegex(wordRegex, str))
            {
                if (str == "var" || str == "print") m_Tokens.emplace_back(TokenType::Keyword, str);
                else m_Tokens.emplace_back(TokenType::Name, str);
            }
            else if (EqualsRegex(numRegex, str))
            {
                m_Tokens.emplace_back(TokenType::Value, str);
            }
            else if (EqualsRegex(symRegex, str))
            {
                m_Tokens.emplace_back(TokenType::Sym, str);
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
                                if (mstr == "var" || mstr == "print") m_Tokens.emplace_back(TokenType::Keyword, mstr);
                                else m_Tokens.emplace_back(TokenType::Name, mstr);
                            }
                            else if (rex == "num")
                            {
                                m_Tokens.emplace_back(TokenType::Value, mstr);
                            }
                            else if (rex == "sym")
                            {
                                m_Tokens.emplace_back(TokenType::Sym, mstr);
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
}