#pragma once

#include "core.h"
#include "token.h"

namespace Dern
{
    class TokenData
    {
    public:
        TokenData(const std::vector<Token>& data);
        ~TokenData();

        int Count() const { return m_Count; }
        const Scope<Token>& operator[](int index) const { return m_Data[index]; }
        const Scope<Token>& At(int index) const { return m_Data[index]; }
    private:
        int m_Count;
        Scope<Token>* m_Data;
    };
}