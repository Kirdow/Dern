#pragma once

#include "tokendata.h"
#include "code.h"

namespace Dern
{
    class Tokenizer
    {
    public:
        Tokenizer(const Scope<Code>& code);
        ~Tokenizer();

        bool Tokenize();

        Scope<TokenData> GetTokenData() const { return CreateScope<TokenData>(m_Tokens); }
    private:
        const Scope<Code>& m_Code;
        std::vector<Token> m_Tokens;
    };
}