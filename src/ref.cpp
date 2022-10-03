#include "ref.h"

#include <mutex>

namespace Dern::RefUtil
{
    static std::unordered_set<void*> s_RefSet;
    static std::mutex s_RefSetMutex;

    void AddReference(void* ref)
    {
        std::scoped_lock lock(s_RefSetMutex);
        s_RefSet.insert(ref);
    }

    void RemoveReference(void* ref)
    {
        std::scoped_lock lock(s_RefSetMutex);
        s_RefSet.erase(ref);
    }

    bool CheckReference(void* ref)
    {
        std::scoped_lock lock(s_RefSetMutex);
        return s_RefSet.find(ref) != s_RefSet.end();
    }

    int CountReferences()
    {
        std::scoped_lock lock(s_RefSetMutex);
        return s_RefSet.size();
    }

    void ValidateReferences()
    {
        int refcount = Dern::RefUtil::CountReferences();
        if (refcount > 0)
        {
            std::cout << "References still counted: " << refcount << "\n";
        }
    }
}