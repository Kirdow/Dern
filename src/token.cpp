#include "pch.h"
#include "token.h"

namespace Dern
{
    int Token::GetDataInt() const
    {
        return std::stoi(m_Data);
    }
}