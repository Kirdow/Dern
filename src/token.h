#pragma once

namespace Dern
{
    enum class TokenType
    {
        Keyword, Name, Value, Sym
    };

    inline const char* TokenTypeToString(TokenType type)
    {
        switch (type)
        {
        case TokenType::Keyword: return "Keyword";
        case TokenType::Name: return "Name";
        case TokenType::Value: return "Value";
        case TokenType::Sym: return "Sym";
        }

        return "Undefined";
    }

    class Token
    {
    public:
        Token(TokenType type, const std::string& data)
            : m_Type(type), m_Data(data) {}

        TokenType GetType() const { return m_Type; }
        const std::string& GetData() const { return m_Data; }
    private:
        TokenType m_Type;
        std::string m_Data;
    };
}

inline bool operator==(const Dern::Token& token, Dern::TokenType type)
{
    return token.GetType() == type;
}

inline bool operator==(Dern::TokenType type, const Dern::Token& token)
{
    return token.GetType() == type;
}

inline bool operator==(const Dern::Token& token, const std::string& text)
{
    return token.GetData() == text;
}

inline bool operator==(const std::string& text, const Dern::Token& token)
{
    return token.GetData() == text;
}

inline std::ostream& operator<<(std::ostream& ostr, const Dern::Token& token)
{
    ostr << "T" << TokenTypeToString(token.GetType()) << "[" << token.GetData() << "]";
    return ostr;
}