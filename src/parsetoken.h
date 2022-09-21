#pragma once

#include <string>

#include "defaultvalue.h"

namespace Dern
{
    enum class PTokenType
    {
        Int, Text, Sym, Var, None
    };

    struct ParseToken
    {
        PTokenType Type;

        ParseToken(PTokenType type) : Type(type) {}

        bool IsType(PTokenType type) const { return Type == type; }
        template<typename T>
        const T* Cast() const { return static_cast<const T*>(this); }
    };

    struct VarToken : public ParseToken
    {
        std::string Value;

        VarToken(const std::string& val) : ParseToken(PTokenType::Var), Value(val) {}
    };

    struct SymToken : public ParseToken
    {
        std::string Value;

        SymToken(const std::string& val) : ParseToken(PTokenType::Sym), Value(val) {}
    };

    struct NumberToken : public ParseToken
    {
        int Value;
        
        NumberToken(int val) : ParseToken(PTokenType::Int), Value(val) {}
    };

    struct TextToken : public ParseToken
    {
        std::string Value;

        TextToken(const std::string& val) : ParseToken(PTokenType::Text), Value(val) {}
    };
}

inline std::ostream& operator<<(std::ostream& ostr, const Dern::ParseToken& token)
{
    if (token.IsType(Dern::PTokenType::Int))
    {
        ostr << "PInt[" << token.Cast<Dern::NumberToken>()->Value << "]";
    }
    else if (token.IsType(Dern::PTokenType::Text))
    {
        ostr << "PText[" << token.Cast<Dern::TextToken>()->Value << "]";
    }
    else if (token.IsType(Dern::PTokenType::Var))
    {
        ostr << "PVar[" << token.Cast<Dern::VarToken>()->Value << "]";
    }
    else if (token.IsType(Dern::PTokenType::Sym))
    {
        ostr << "PSym[" << token.Cast<Dern::SymToken>()->Value << "]";
    }
    else if (token.IsType(Dern::PTokenType::None))
    {
        ostr << "PNone";
    }

    return ostr;
}