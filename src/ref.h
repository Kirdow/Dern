#pragma once

namespace Dern
{
    namespace RefUtil
    {
        void AddReference(void* ref);
        void RemoveReference(void* ref);
        bool CheckReference(void* ref);
        int CountReferences();
        void ValidateReferences();
    }

    template<typename T>
    class Ref
    {
    public:
        Ref()
            : m_Instance(nullptr), m_RefCount(nullptr)
        {}

        Ref(std::nullptr_t n)
            : m_Instance(nullptr), m_RefCount(nullptr)
        {}

        Ref(T* instance)
            : m_Instance(instance), m_RefCount(((int*)instance) - 1)
        {
            IncRef();
        }

        template<typename T1>
        Ref(const Ref<T1>& other)
        {
            m_Instance = static_cast<T*>(other.m_Instance);
            m_RefCount = other.m_RefCount;
            IncRef();
        }

        template<typename T1>
        Ref(Ref<T1>&& other)
        {
            m_Instance = static_cast<T*>(other.m_Instance);
            m_RefCount = other.m_RefCount;
            other.m_Instance = nullptr;
            other.m_RefCount = nullptr;
        }

        ~Ref()
        {
            DecRef();
        }

        Ref(const Ref<T>& other)
            : m_Instance(other.m_Instance), m_RefCount(other.m_RefCount)
        {
            IncRef();
        }

        Ref& operator=(std::nullptr_t)
        {
            DecRef();
            m_Instance = nullptr;
            m_RefCount = nullptr;
            return *this;
        }

        Ref& operator=(const Ref<T>& other)
        {
            other.IncRef();
            DecRef();

            m_Instance = other.m_Instance;
            m_RefCount = other.m_RefCount;
            return *this;
        }

        template<typename T1>
        Ref& operator=(const Ref<T1>& other)
        {
            other.IncRef();
            DecRef();

            m_Instance = other.m_Instance;
            m_RefCount = other.m_RefCount;
            return *this;
        }

        template<typename T1>
        Ref& operator=(Ref<T1>&& other)
        {
            DecRef();

            m_Instance = other.m_Instance;
            m_RefCount = other.m_RefCount;
            other.m_Instance = nullptr;
            other.m_RefCount = nullptr;
            return *this;
        }

        operator bool() { return m_Instance != nullptr && m_RefCount != nullptr; }
        operator bool() const { return m_Instance != nullptr && m_RefCount != nullptr; }

        T* operator->() { return m_Instance; }
        T* operator->() const { return m_Instance; }

        T& operator*() { return *m_Instance; }
        T& operator*() const { return *m_Instance; }

        T* Raw() { return m_Instance; }
        T* Raw() const { return m_Instance; }

        template<typename T1>
        Ref<T1> Cast() const
        {
            return Ref<T1>(*this);
        }

        bool operator==(const Ref<T>& other) const
        {
            return m_Instance == other.m_Instance;
        }

        bool operator!=(const Ref<T>& other) const
        {
            return m_Instance != other.m_Instance;
        }
    private:
        void IncRef() const
        {
            if (m_RefCount && m_Instance)
            {
                (*m_RefCount)++;
            }
        }

        void DecRef() const
        {
            if (m_RefCount && m_Instance)
            {
                (*m_RefCount)--;
                if (*m_RefCount == 0)
                {
                    m_Instance->~T();
                    RefUtil::RemoveReference(m_Instance);
                    Free(m_RefCount);

                    m_Instance = nullptr;
                    m_RefCount = nullptr;
                }
            }
        }

        inline static void* Alloc()
        {
            return (void*)(new char[sizeof(T) + sizeof(int)]);
        }

        inline static void Free(void* ptr)
        {
            if (ptr == nullptr) return;

            char* cptr = (char*)ptr;
            delete[] cptr;
        }

        template<typename T1>
        friend class Ref;
    public:
        template<typename ... Args>
        static Ref<T> Create(Args&& ... args)
        {
            void* ptr = Alloc();
            T* tPtr = (T*)(((char*)ptr) + (int)(sizeof(int)));
            *((int*)ptr) = 0;
            RefUtil::AddReference(tPtr);
            new(tPtr) T(std::forward<Args>(args)...);
            return Ref<T>(tPtr);
        }
    private:
        mutable T* m_Instance;
        mutable int* m_RefCount;
    };

    template<typename T>
    class WeakRef
    {
    public:
        WeakRef() = default;

        WeakRef(Ref<T> ref)
            : m_Instance(ref.Raw())
        {}

        WeakRef(T* inst)
            : m_Instance(inst)
        {}

        T* operator->() const { return m_Instance; }
        T& operator*() const { return *m_Instance; }
        const T* Raw() const { return m_Instance; }

        bool IsGood() const
        {
            if (!m_Instance) return false;

            return RefUtil::CheckReference(m_Instance);
        }

        operator bool() const { return IsGood(); }
    private:
        T* m_Instance = nullptr;
    };
}
