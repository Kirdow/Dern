#include "tokendata.h"

namespace Dern
{
    TokenDataEOF TokenDataEOF::CreateGood()
    {
        return TokenDataEOF(-1, -1);
    }

    TokenData::TokenData(const std::vector<Token>& data)
    {
        m_Count = data.size();
        m_Data = new Ref<Token>[m_Count];
        for (int i = 0; i < m_Count; i++)
        {
            auto& token = data[i];
            m_Data[i] = Ref<Token>::Create(token.GetType(), token.GetData());
        }
    }

    TokenData::~TokenData()
    {
        m_Count = 0;
        delete[] m_Data;
    }

    void TokenData::PollEOF(int index) const
    {
        if (index == m_Count) throw TokenDataEOF::CreateGood();
    }
}