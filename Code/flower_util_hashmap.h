#ifndef FLOWER_UTIL_HASHMAP_H
#define FLOWER_UTIL_HASHMAP_H

#include "flower_mem.h"
#include "flower_strings.h"


template <typename t>
struct flower_hashmap_entry
{
    flower_hashmap_entry* NextInHash;
    u32 CachedHash;
    
    t Data;
    
    flower_hashmap_entry* NextInList;
    flower_hashmap_entry* PrevInList;
};


template <typename t, int MapSize=1024>
struct FlowerHashMap
{
    flower_hashmap_entry<t>* m_MapArray[MapSize];
    memory_arena* m_Arena;
    
    flower_hashmap_entry<t>* m_SentinelUse;
    flower_hashmap_entry<t>* m_SentinelFree;
    
    FlowerHashMap() = delete;
    
    explicit FlowerHashMap(memory_arena* Arena)
    {
        m_Arena = Arena;
        
        m_SentinelUse = PushStruct(Arena, flower_hashmap_entry<t>);
        m_SentinelFree = PushStruct(Arena, flower_hashmap_entry<t>);
        
        // NOTE(Dima): Reflecting Use Sentinel pointers
        m_SentinelUse->NextInList = m_SentinelUse;
        m_SentinelUse->PrevInList = m_SentinelUse;
        
        // NOTE(Dima): Reflecting Free Sentinel pointers
        m_SentinelFree->NextInList = m_SentinelFree;
        m_SentinelFree->PrevInList = m_SentinelFree;
        
        // NOTE(Dima): Grow number of allocated entries
        growAllocatedCount();
        
        // NOTE(Dima): Clear table
        for(int i = 0; i < MapSize; i++)
        {
            m_MapArray[i] = nullptr;
        }
    }
    
    
    void growAllocatedCount()
    {
        int ToAllocate = 256;
        
        flower_hashmap_entry<t>* NewEntries = PushArray(m_Arena, 
                                                        flower_hashmap_entry<t>, 
                                                        ToAllocate);
        
        for(int i = 0; i < ToAllocate; i++)
        {
            flower_hashmap_entry<t>* Entry = NewEntries + i;
            
            // NOTE(Dima): Inserting entry to free list
            Entry->PrevInList = m_SentinelFree;
            Entry->NextInList = m_SentinelFree->NextInList;
            
            Entry->NextInList->PrevInList = Entry;
            Entry->PrevInList->NextInList = Entry;
        }
    }  
    
    
    flower_hashmap_entry<t>* allocateHashMapEntry()
    {
        if (m_SentinelFree->NextInList == m_SentinelFree)
        {
            growAllocatedCount();
        }
        
        flower_hashmap_entry<t>* Entry = m_SentinelFree->NextInList;
        
        // NOTE(Dima): Removing entry from Free list
        Entry->NextInList->PrevInList = Entry->PrevInList;
        Entry->PrevInList->NextInList = Entry->NextInList;
        
        // NOTE(Dima): Inserting entry to Use list
        Entry->NextInList = m_SentinelUse->NextInList;
        Entry->PrevInList = m_SentinelUse;
        
        Entry->NextInList->PrevInList = Entry;
        Entry->PrevInList->NextInList = Entry;
        
        Entry->NextInHash = nullptr;
        
        return (Entry);
    }
    
    
    void deallocateHashMapEntry(flower_hashmap_entry<t>* Entry)
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
    
    
    // NOTE(Dima): Insert with key hash and data
    void insert(u32 KeyHash, const t& Value)
    {
        u32 Key = KeyHash % MapSize;
        
        flower_hashmap_entry<t>* New = allocateHashMapEntry();
        
        New->NextInHash = m_MapArray[Key];
        New->CachedHash = KeyHash;
        New->Data = Value;
        
        m_MapArray[Key] = New;
    }
    
    
    // NOTE(Dima): Find internal entry by key hash
    flower_hashmap_entry<t>* findInternal(u32 KeyHash)
    {
        u32 Key = KeyHash % MapSize;
        
        flower_hashmap_entry<t>* ResultEntry = 0;
        
        flower_hashmap_entry<t>* At = m_MapArray[Key];
        while (At != nullptr)
        {
            if (At->CachedHash == KeyHash)
            {
                ResultEntry = At;
                
                break;
            }
            
            At = At->NextInHash;
        }
        
        return ResultEntry;
    }
    
    
    // NOTE(Dima): Find by key hash
    t* find(u32 KeyHash)
    {
        flower_hashmap_entry<t>* ResEntry = findInternal(KeyHash);
        
        t* Result = nullptr;
        
        if(ResEntry)
        {
            Result = &ResEntry->Data;
        }
        
        return (Result);
    }
    
    
#if 0    
    // NOTE(Dima): Find by char string key
    t* find(char* Key)
    {
        u32 KeyHash = StringHashFNV(Key);
        
        return find(KeyHash);
    }
#endif
    
    
    // NOTE(Dima): Remove entry with hash
    void removeWithHash(u32 KeyHash)
    {
        u32 Key = KeyHash % MapSize;
        
        flower_hashmap_entry<t>* Prev = nullptr;
        flower_hashmap_entry<t>* Found = nullptr;
        flower_hashmap_entry<t>* At = m_MapArray[Key];
        
        while (At != nullptr)
        {
            if (At->CachedHash == KeyHash)
            {
                Found = At;
                
                break;
            }
            
            Prev = At;
            At = At->NextInHash;
        }
        
        // NOTE(Dima): If we found the element with this hash -> then remove
        if (Found)
        {
            if (Prev)
            {
                Prev->NextInHash = Found->NextInHash;
            }
            else
            {
                m_MapArray[Key] = Found->NextInHash;
            }
            
            deallocateHashMapEntry(Found);
        }
    }
    
    
    // NOTE(Dima): Clear all hash table
    void clear()
    {
        // NOTE(Dima): NULL-ify table 
        for(int i = 0; i < MapSize; i++)
        {
            flower_hashmap_entry<t>* At = m_MapArray[i];
            
            while (At != nullptr)
            {
                flower_hashmap_entry<t>* ToDealloc = At;
                At = At->NextInHash;
                
                deallocateHashMapEntry(ToDealloc);
            }
            
            m_MapArray[i] = nullptr;
        }
    }
    
    
};

#endif //FLOWER_UTIL_HASHMAP_H
