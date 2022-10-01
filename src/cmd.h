#pragma once

namespace Dern
{
    template<typename CmdLineType>
    class CmdLineIterator
    {
    public:
        CmdLineIterator(CmdLineType* ptr, size_t index)
            : m_Ptr(ptr), m_Index(index) {}
        CmdLineIterator(const CmdLineIterator& other)
            : m_Ptr(other.m_Ptr), m_Index(other.m_Index) {}

        CmdLineIterator& operator++()
        {
            m_Index++;

            return *this;
        }

        CmdLineIterator& operator--()
        {
            m_Index--;

            return *this;
        }

        CmdLineIterator operator++(int)
        {
            CmdLineIterator iterator = *this;
            ++m_Index;
            return iterator;
        }

        CmdLineIterator operator--(int)
        {
            CmdLineIterator iterator = *this;
            --m_Index;
            return iterator;
        }

        CmdLineIterator operator+(int i)
        {
            return CmdLineIterator(m_Ptr, m_Index + i);
        }

        CmdLineIterator operator-(int i)
        {
            return CmdLineIterator(m_Ptr, m_Index - i);
        }

        std::string operator[](int index)
        {
            return m_Ptr->Get(m_Index + index);
        }

        std::string operator*()
        {
            return m_Ptr->Get(m_Index);
        }

        const std::string& Get() const
        {
            return m_Ptr->Get(m_Index);
        }

        bool GetNext(std::string& result) const
        {
            return m_Ptr->GetNext(m_Index, result);
        }

        bool operator==(const CmdLineIterator& other) const
        {
            return m_Ptr == other.m_Ptr && m_Index == other.m_Index;
        }

        bool operator!=(const CmdLineIterator& other) const
        {
            return !(*this == other);
        }
    private:
        CmdLineType* m_Ptr;
        size_t m_Index;
    };

    class CmdLine
    {
    public:
        using Iterator = CmdLineIterator<CmdLine>;
    public:
        CmdLine(int argc, char** argv);
        ~CmdLine();

        size_t GetCount() const { return m_Argc; }
        const std::string& Get(size_t index) const { return m_Args.at(index); }

        bool GetNext(size_t index, std::string& result) const;

        size_t Size() const { return m_Argc; }

        Iterator begin()
        {
            return Iterator(this, 0);
        }

        Iterator end()
        {
            return Iterator(this, m_Argc);
        }
    private:
        friend class CmdLineIterator<CmdLine>;
    private:
        size_t m_Argc;
        std::vector<std::string> m_Args;
    };
}