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
        const Ref<Token>& operator[](int index) const { return m_Data[index]; }
        const Ref<Token>& At(int index) const { return m_Data[index]; }
    private:
        int m_Count;
        Ref<Token>* m_Data;
    };
}