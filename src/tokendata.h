#pragma once

#include "core.h"
#include "token.h"

namespace Dern
{
    class TokenDataEOF
    {
    public:
        TokenDataEOF(int index, int count) : m_Index(index), m_Count(count) {}

        static TokenDataEOF CreateGood();

        bool IsGood() const { return m_Index == -1 && m_Count == -1; }
        int GetIndex() const { return m_Index; }
        int GetCount() const { return m_Count; }
    private:
        int m_Index;
        int m_Count;
    };

    class TokenData
    {
    public:
        TokenData(const std::vector<Token>& data);
        ~TokenData();

        void PollEOF(int index) const;

        int Count() const { return m_Count; }
        const Ref<Token>& At(int index) const
        {
            if (index >= m_Count) throw TokenDataEOF(index, m_Count);
            return m_Data[index];
        }
    private:
        int m_Count;
        Ref<Token>* m_Data;
    };
}