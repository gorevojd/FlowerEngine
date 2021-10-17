#ifndef FLOWER_UTIL_LIST_H
#define FLOWER_UTIL_LIST_H

#include "flower_mem.h"

template <typename t>
struct dlist_entry
{
    t Data;
    
    dlist_entry<t>* Next;
    dlist_entry<t>* Prev;
};

template <typename t, int ToGrow = 128>
struct dlist
{
    dlist_entry<t>* m_SentinelUse;
    dlist_entry<t>* m_SentinelFree;
    
    memory_arena* m_Arena;
    
    dlist() = delete;
    
    explicit dlist(memory_arena* Arena)
    {
        m_Arena = Arena;
        
        m_SentinelUse = PushStruct(Arena, dlist_entry<t>);
        m_SentinelFree = PushStruct(Arena, dlist_entry<t>);
        
        // NOTE(Dima): Reflecting Use Sentinel pointers
        m_SentinelUse->Next = m_SentinelUse;
        m_SentinelUse->Prev = m_SentinelUse;
        
        // NOTE(Dima): Reflecting Free Sentinel pointers
        m_SentinelFree->Next = m_SentinelFree;
        m_SentinelFree->Prev = m_SentinelFree;
        
        // NOTE(Dima): Grow number of allocated entries
        growAllocatedCount();
    }
    
    bool empty()
    {
        return m_SentinelFree->Next == m_SentinelFree;
    }
    
    void growAllocatedCount()
    {
        int ToAllocate = ToGrow;
        
        dlist_entry<t>* NewEntries = PushArray(m_Arena, 
                                               dlist_entry<t>, 
                                               ToAllocate);
        
        for(int i = 0; i < ToAllocate; i++)
        {
            dlist_entry<t>* Entry = NewEntries + i;
            
            // NOTE(Dima): Inserting entry to free list
            Entry->Prev = m_SentinelFree;
            Entry->Next = m_SentinelFree->Next;
            
            Entry->Next->Prev = Entry;
            Entry->Prev->Next = Entry;
        }
    }  
    
    dlist_entry<t>* allocate()
    {
        if (empty())
        {
            growAllocatedCount();
        }
        
        dlist_entry<t>* Entry = m_SentinelFree->Next;
        
        // NOTE(Dima): Removing entry from Free list
        Entry->Next->Prev = Entry->Prev;
        Entry->Prev->Next = Entry->Next;
        
        // NOTE(Dima): Inserting entry to Use list
        Entry->Next = m_SentinelUse->Next;
        Entry->Prev = m_SentinelUse;
        
        Entry->Next->Prev = Entry;
        Entry->Prev->Next = Entry;
        
        return (Entry);
    }
    
    void deallocate(dlist_entry<t>* Entry)
    {
        // NOTE(Dima): Removing entry from Use list
        Entry->NextInList->PrevInList = Entry->PrevInList;
        Entry->PrevInList->NextInList = Entry->NextInList;
        
        // NOTE(Dima): Inserting entry to Free list
        Entry->NextInList = m_SentinelFree->NextInList;
        Entry->PrevInList = m_SentinelFree;
        
        Entry->NextInList->PrevInList = Entry;
        Entry->PrevInList->NextInList = Entry;
    }
    
    void deallocateAll()
    {
        if (!empty())
        {
            dlist_entry* FirstUse = m_SentinelUse.Next;
            dlist_entry* LastUse = m_SentinelUse.Prev;
            
            // NOTE(Dima): Reflecting Use ptrs
            m_SentinelUse->Prev = m_SentinelUse;
            m_SentinelUse->Next = m_SentinelUse;
            
            // NOTE(Dima): Inserting all elements to Free list
            FirstUse->Prev = m_SentinelFree;
            LastUse->Next = m_SentinelFree->Next;
            
            FirstUse->Prev->Next = FirstUse;
            LastUse->Next->Prev = LastUse;
        }
    }
};

#endif //FLOWER_UTIL_LIST_H
