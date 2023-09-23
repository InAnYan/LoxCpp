#include "Lox/Runtime/MemoryManager.hpp"

#include <iostream>

#include "Lox/Configuration.hpp"

namespace Lox
{
    MemoryManager::MemoryManager(RootsSource& roots)
            : roots(roots), objects(), allowedGC(false)
    {
    
    }
    
    MemoryManager::~MemoryManager()
    {
        GcPtr<Object> obj = objects;
        while (obj)
        {
            GcPtr<Object> next = obj->GetNext();
            DeleteObject(obj);
            obj = next;
        }
    }
    
    void MemoryManager::CollectGarbageIfNeeded()
    {
        if (!allowedGC)
        {
            return;
        }
        
        if constexpr (Configuration::StressGC)
        {
            CollectGarbage();
        }
        else
        {
            LOX_UNIMPLEMENTED("Lox::MemoryManager no stress GC");
        }
    }
    
    void MemoryManager::AllowGC()
    {
        allowedGC = true;
    }
    
    void MemoryManager::DisallowGC()
    {
        allowedGC = false;
    }
    
    void MemoryManager::CollectGarbage()
    {
        LogStages("Begin");
        
        MarkStage();
        SweepStage();
        
        // UpdateNextGC();
        
        LogStages("End");
    }
    
    void MemoryManager::MarkStage()
    {
        LogStages("MarkStage Begin");
        
        roots.MarkRoots();
        
        LogStages("MarkStage End");
    }
    
    void MemoryManager::SweepStage()
    {
        LogStages("SweepStage Begin");
        
        GcPtr<Object> prev;
        GcPtr<Object> obj = objects;
        
        while (obj)
        {
            GcPtr<Object> next = obj->GetNext();
            
            if (obj->IsMarked())
            {
                obj->Unmark();
                prev = obj;
                obj = next;
            }
            else
            {
                if (!prev)
                {
                    objects = next;
                }
                else
                {
                    prev->SetNext(next);
                }
                
                DeleteObject(obj);
                
                obj = next;
            }
        }
        
        LogStages("SweepStage End");
    }
    
    void MemoryManager::DeleteObject(GcPtr<Object> obj)
    {
        LogObject("DeleteObject", obj);
        obj.Delete();
    }
    
    void MemoryManager::MarkObject(GcPtr<Object> obj)
    {
        if (!obj.IsNullptr() && !obj->IsMarked())
        {
            LogObject("Mark", obj);
            obj->SetMarked();
            obj->MarkChildren();
        }
    }
    
    void MemoryManager::MarkValue(Value val)
    {
        if (val.IsObject())
        {
            MarkObject(val.AsObject());
        }
    }
    
    void MemoryManager::LogObject(const char* str, GcPtr<Object> obj)
    {
        if constexpr (!Configuration::LogGC)
        {
            return;
        }
        
        Log(str);
        std::cout << obj->GetType() << ' ';
        obj.PrintPointerValue(std::cout) << std::endl;
    }
    
    void MemoryManager::LogStages(const char* str)
    {
        if constexpr (!Configuration::LogGC)
        {
            return;
        }
        
        Log(str) << std::endl;
    }
    
    std::ostream& MemoryManager::Log(const char* str)
    {
        return std::cout << "- GC: " << str << ' ';
    }
}