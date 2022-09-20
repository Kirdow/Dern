#pragma once

namespace Dern
{
    enum class TokenType
    {
        Keyword, Name, Sym, Int, Text
    };

    inline const char* TokenTypeToString(TokenType type)
    {
        switch (type)
        {
        case TokenType::Keyword: return "Keyword";
        case TokenType::Name: return "Name";
        case TokenType::Sym: return "Sym";
        case TokenType::Int: return "Int";
        case TokenType::Text: return "Text";
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

        bool IsType(TokenType type) const { return m_Type == type; }
        bool IsValue(const std::string& value) const { return m_Data == value; }

        int GetDataInt() const;
    private:
        TokenType m_Type;
        std::string m_Data;
    };
}

inline std::ostream& operator<<(std::ostream& ostr, const Dern::Token& token)
{
    ostr << "T" << TokenTypeToString(token.GetType()) << "[" << token.GetData() << "]";
    return ostr;
}