#include "pch.h"
#include "tokendata.h"

namespace Dern
{
    TokenData::TokenData(const std::vector<Token>& data)
    {
        m_Count = data.size();
        m_Data = new Scope<Token>[m_Count];
        for (int i = 0; i < m_Count; i++)
        {
            auto& token = data[i];
            m_Data[i].reset(new Token(token.GetType(), token.GetData()));
        }
    }

    TokenData::~TokenData()
    {
        m_Count = 0;
        delete[] m_Data;
    }
}