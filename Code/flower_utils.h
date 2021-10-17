/* date = October 16th 2021 8:08 pm */

#ifndef FLOWER_UTILS_H
#define FLOWER_UTILS_H

#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "flower_util_hashmap.h"
#include "flower_util_list.h"


struct helper_byte_buffer
{
    void* Data;
    mi DataSize;
    b32 UsedCustomAllocator;
    
    std::unordered_map<std::string, mi> NameToOffset;
    std::unordered_map<std::string, int> NameToCount;
    
    helper_byte_buffer()
    {
        Data = 0;
        DataSize = 0;
        UsedCustomAllocator = false;
    }
    
    u32 AddPlace(std::string Name, 
                 int NumElements, 
                 mi SizeOfElement)
    {
        u32 Offset = 0;
        
        NameToCount.insert(std::pair<std::string, int>(Name, NumElements));
        
        if(NumElements > 0)
        {
            Offset = DataSize;
            
            NameToOffset.insert(std::pair<std::string, mi>(Name, Offset));
            
            DataSize += NumElements * SizeOfElement;
        }
        
        return(Offset);
    }
    
    void Generate(void* CustomPlace = 0)
    {
        if(CustomPlace != 0)
        {
            UsedCustomAllocator = true;
            
            Data = CustomPlace;
        }
        else
        {
            Data = malloc(DataSize);
        }
    }
    
    void Free()
    {
        if(!UsedCustomAllocator)
        {
            if(Data != 0)
            {
                free(Data);
            }
            Data = 0;
        }
    }
    
    void* GetPlace(std::string GetName)
    {
        void* Result = 0;
        
        if(NameToCount.find(GetName) != NameToCount.end())
        {
            if(NameToCount[GetName] > 0)
            {
                Assert(NameToOffset.find(GetName) != NameToOffset.end());
                
                Result = (void*)((u8*)Data + NameToOffset[GetName]);
            }
        }
        
        return(Result);
    }
};


struct helper_mesh
{
    std::string Name;
    
    std::vector<v3> Vertices;
    std::vector<v2> TexCoords;
    std::vector<v3> Normals;
    std::vector<v3> Colors;
    std::vector<u32> Indices;
    std::vector<v4> BoneWeights;
    std::vector<u32> BoneIndices;
    
    b32 IsSkinned;
};


#endif //FLOWER_UTILS_H
