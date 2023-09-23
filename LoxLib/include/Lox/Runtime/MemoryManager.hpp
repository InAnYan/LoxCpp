#ifndef LOX_VM_RUNTIME_MEMORY_MANAGER_HPP
#define LOX_VM_RUNTIME_MEMORY_MANAGER_HPP

#include <functional>
#include <ostream>

#include "GcPtr.hpp"
#include "Object.hpp"
#include "ValueType.hpp"
#include "Value.hpp"
#include "RootsSource.hpp"

namespace Lox
{
    class MemoryManager
    {
    public:
        MemoryManager(RootsSource& roots);
        
        ~MemoryManager();
        
        template <typename T, typename... Args>
        GcPtr<T> AllocateObject(Args&& ... args)
        {
            CollectGarbageIfNeeded();
            
            T* objRawPtr = new T(objects, std::forward<Args&&>(args)...);
            GcPtr<T> obj(objRawPtr);
            LogObject("AllocateObject", obj);
            
            objects = obj;
            
            return obj;
        }
        
        void CollectGarbageIfNeeded();
        
        void CollectGarbage();
        
        static void LogObject(const char* str, GcPtr<Object> obj);
        
        static void LogStages(const char* str);
        
        // NOTE: Actually the solution of using allow/disallow GC
        // doesn't work with threads. Probably.
        
        void AllowGC();
        
        void DisallowGC();
        
        static void MarkObject(GcPtr<Object> obj);
        
        static void MarkValue(Value val);
    
    private:
        RootsSource& roots;
        GcPtr<Object> objects;
        
        bool allowedGC;
        
        void MarkStage();
        
        void SweepStage();
        
        static void DeleteObject(GcPtr<Object> obj);
        
        static std::ostream& Log(const char* str);
    };
}

#endif // LOX_VM_RUNTIME_MEMORY_MANAGER_HPP
