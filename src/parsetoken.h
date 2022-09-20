#pragma once

#include <string>

#include "defaultvalue.h"

namespace Dern
{
    enum class PTokenType
    {
        Int, Text, Sym, Var
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