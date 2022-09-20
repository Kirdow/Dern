#pragma once

#include <functional>

#include "core.h"
#include "storedvalue.h"
#include "registry.h"
#include "token.h"
#include "parsetoken.h"

namespace Dern
{
    struct ParseMem
    {
        int count;
        int* mem;

        ParseMem(int n)
            : count(n), mem(nullptr)
        {
            mem = new int[n];
            for (int i = 0; i < count; i++)
                mem[i] = 0;
        }

        ~ParseMem()
        {
            delete[] mem;
            count = 0;
            mem = 0;
        }

        int& operator[](int index) { return mem[index]; }
        int operator[](int index) const { return mem[index]; }
    };

    class TypeParser
    {
    public:
        TypeParser(Registry& reg, int n)
            : m_Count(n), m_Reg(reg), m_Mem(n) {}

    public:
        int Count() const { return m_Count; }
        Ref<StoredValue> ComputeValue(std::function<Ref<Token>(ParseMem&)> nextFn);
    private:
        Ref<ParseToken> ComputeValue(std::vector<Ref<ParseToken>> v);
    private:
        int m_Count;
        Registry& m_Reg;
        ParseMem m_Mem;
    };
}