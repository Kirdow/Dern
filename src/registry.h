#pragma once

#include <string>
#include <unordered_map>

#include "core.h"
#include "defaultvalue.h"
#include "storedvalue.h"

namespace Dern
{
    class Registry
    {
    public:
        Registry(const Ref<Registry>& parent) : m_Parent(parent) {}
        ~Registry() {}

        bool HasAnyEntry(const std::string& name) const
        {
            if (m_Entries.find(name) == m_Entries.end())
                return false;

            const auto& entry = m_Entries.at(name);
            if (!entry->HasName(name)) return false;

            return entry->HasData();
        }

        template<typename T>
        bool HasEntry(const std::string& name) const
        {
            if (m_Entries.find(name) == m_Entries.end())
            {
                if (m_Parent != nullptr)
                    return m_Parent->HasEntry<T>(name);

                return false;
            }

            const auto& entry = m_Entries.at(name);
            if (!entry->HasName(name)) return false;

            return entry->IsOfType<T>();
        }

        template<typename T>
        T GetEntry(const std::string& name) const
        {
            if (m_Entries.find(name) == m_Entries.end())
            {
                if (m_Parent != nullptr)
                    return m_Parent->GetEntry<T>(name);

                return Defaults::GetDefault<T>();
            }

            const auto& entry = m_Entries.at(name);
            if (!entry->HasName(name)) return Defaults::GetDefault<T>();

            return entry->GetData<T>();
        }

        template<typename T>
        void SetEntry(const std::string& name, const T& data, bool definition = true)
        {
            if (!definition)
            {
                if (m_Entries.find(name) == m_Entries.end())
                {
                    if (m_Parent == nullptr)
                        throw "Undefined variable referenced";

                    m_Parent->SetEntry<T>(name, data, definition);
                    return;
                }

                const auto& entry = m_Entries.at(name);
                if (!entry->HasName(name)) return;
                entry->SetData<T>(data);
                return;
            }

            if (m_Entries.find(name) == m_Entries.end())
                m_Entries[name] = Ref<StoredValue>::Create(name);

            const auto& entry = m_Entries.at(name);
            if (!entry->HasName(name)) return;
            entry->SetData<T>(data);
        }

        void UnsetEntry(const std::string& name)
        {
            if (m_Entries.find(name) == m_Entries.end()) return;

            const auto& entry = m_Entries.at(name);
            if (!entry->HasName(name)) return;
            entry->UnsetData();
        }
    public:
        void SetParent(const Ref<Registry>& parent) { m_Parent = parent; }
        Ref<Registry> GetParent() const { return m_Parent; }
    public:
        static Ref<Registry> Create(Ref<Registry> parent = nullptr) { return Ref<Registry>::Create(parent); }
    private:
        Ref<Registry> m_Parent;
        std::unordered_map<std::string, Ref<StoredValue>> m_Entries;
    };
}