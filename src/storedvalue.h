#pragma once

#include <string>

#include "defaultvalue.h"

namespace Dern
{
    class StoredValue
    {
    public:
        StoredValue() : m_Data(nullptr) {}
        StoredValue(const std::string& name) : m_Name(name), m_Data(nullptr) {}
        ~StoredValue() { Release(); }

        std::string& name() { return m_Name; }
        const std::string& name() const { return m_Name; }
        bool HasName(const std::string& name) const { return m_Name == name; }

        bool HasData() const
        {
            return m_Data != nullptr && !m_TypeName.empty();
        }

        template<typename T>
        bool IsOfType() const
        {
            std::string typeName = typeid(T).name();
            return m_Data != nullptr && typeName == m_TypeName;
        }

        template<typename T>
        T GetData() const
        {
            if (!IsOfType<T>()) return Defaults::GetDefault<T>();

            return *static_cast<T*>(m_Data);
        }

        template<typename T>
        void SetData(const T& data)
        {
            Release();

            T* store = new T(data);
            m_Data = store;
            m_TypeName = typeid(T).name();
        }

        void UnsetData()
        {
            Release();
        }

        std::string ToString() const;
    private:
        void Release()
        {
            if (m_Data != nullptr)
            {
                unsigned char* data = static_cast<unsigned char*>(m_Data);
                delete data;
                m_Data = 0;
                m_TypeName = "";
            }
        }
    private:
        std::string m_Name;
        void* m_Data;
        std::string m_TypeName;
    };
}