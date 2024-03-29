#ifndef FLOWER_RENDER_PRIMITIVES_H
#define FLOWER_RENDER_PRIMITIVES_H

#include "flower_asset_shared.h"

enum renderer_handle_type
{
    RendererHandle_Invalid,
    
    RendererHandle_Image,
    RendererHandle_Mesh,
    RendererHandle_TextureBuffer,
    RendererHandle_Cubemap,
};

struct renderer_handle
{
    b32 Initialized;
    b32 Invalidated;
    u32 Type;
    
    union
    {
        struct
        {
            u32 TextureObject;
        } Image;
        
        struct 
        {
            u32 BufferObject;
            u32 TextureObject;
        } TextureBuffer;
        
        struct 
        {
            u64 ArrayObject;
            u64 BufferObject;
            u64 ElementBufferObject;
        } Mesh;
        
        struct 
        {
            u32 Handle;
        } Cubemap;
    };
};

inline void InitRendererHandle(renderer_handle* Handle, u32 Type)
{
    Handle->Initialized = false;
    Handle->Invalidated = false;
    Handle->Type = Type;
}

inline renderer_handle CreateRendererHandle(u32 Type)
{
    renderer_handle Result = {};
    
    InitRendererHandle(&Result, Type);
    
    return(Result);
}

inline b32 ShouldDeleteHandleStorage(renderer_handle* Handle)
{
    b32 Result = false;
    
    if(Handle->Invalidated && Handle->Initialized)
    {
        Result = true;
    }
    
    return(Result);
}

inline void InvalidateHandle(renderer_handle* Handle)
{
    Handle->Invalidated = true;
}

struct render_mesh_offsets
{
    u32 OffsetP;
    u32 OffsetUV;
    u32 OffsetN;
    u32 OffsetC;
    u32 OffsetBoneWeights;
    u32 OffsetBoneIndices;
};

struct mesh
{
    char Name[64];
    
    // NOTE(Dima): This pointer holds data for all the mesh (this includes vertices and index data)
    void* MeshDataStart;
    mi MeshDataSize;
    
    v3* P;
    v2* UV;
    v3* N;
    u32* C;
    v4* BoneWeights;
    u32* BoneIndices;
    int VertexCount;
    
    u32* Indices;
    int IndexCount;
    
    b32 IsSkinned;
    int MaterialIndexInModel;
    
    renderer_handle Handle;
    render_mesh_offsets Offsets;
};

struct voxel_mesh
{
    u32* Vertices;
    u32* PerFaceData;
    
    int VerticesCount;
    int FaceCount;
    
    void* Free;
    
    renderer_handle Handle;
    renderer_handle PerFaceBufHandle;
};

struct image
{
    void* Pixels;
    
    int Width;
    int Height;
    
    float WidthOverHeight;
    
    b32 FilteringIsClosest;
    v2 Align;
    
    renderer_handle Handle;
};

struct cubemap
{
    image* Left;
    image* Right;
    image* Down;
    image* Top;
    image* Front;
    image* Back;
    
    renderer_handle Handle;
};

struct material
{
    char Name[64];
    
    u32 Type;
    
    image* Textures[MAX_MATERIAL_TEXTURES];
};

struct node_animation
{
    v3* PositionKeys;
    quat* RotationKeys;
    v3* ScalingKeys;
    
    f32* PositionTimes;
    f32* RotationTimes;
    f32* ScalingTimes;
    
    int NumPos;
    int NumRot;
    int NumScl;
    
    int NodeIndex;
};

struct animation
{
    char Name[64];
    
    node_animation* NodeAnims;
    int NumNodeAnims;
    
    u32 Behaviour;
    
    f32 DurationTicks;
    f32 TicksPerSecond;
};

enum font_size_type
{
    FontSize_Small,
    FontSize_Medium,
    FontSize_Large,
    FontSize_ExtraLarge,
    
    FontSize_Count,
};

enum font_glyph_style
{
    GlyphStyle_Original = BIT_FLAG(0),
    GlyphStyle_Shadow = BIT_FLAG(1),
    GlyphStyle_Outline = BIT_FLAG(2),
};

// TODO(Dima): Use Stb rect pack to pack glyphs into atlas

static int Global_FontSizes[FontSize_Count] = 
{
    10,
    25,
    60,
    150,
};

struct glyph_style
{
    u32 GlyphStyleType;
    
    f32 WidthOverHeight;
    int ImageHeight;
    
    v2 MinUV;
    v2 MaxUV;
};

struct glyph
{
    glyph_style* Styles;
    int NumStyles;
    
    u32 Codepoint;
    
    f32 Advance;
    f32 LeftBearing;
    
    f32 XOffset;
    f32 YOffset;
};

inline int RequestGlyphStyle(glyph* Glyph, u32 DesiredGlyphStyle)
{
    Assert(Glyph->NumStyles > 0);
    
    int Result = 0;
    
    for (int GlyphStyleIndex = 0;
         GlyphStyleIndex < Glyph->NumStyles;
         GlyphStyleIndex++)
    {
        glyph_style* Style = &Glyph->Styles[GlyphStyleIndex];
        
        if(Style->GlyphStyleType == DesiredGlyphStyle)
        {
            Result = GlyphStyleIndex;
            break;
        }
    }
    
    return Result;
}

struct font_codepoint_slot_range
{
    int Count;
    int StartIndexInSlotsGlyphsIds;
};

struct font_slot_glyph_id
{
    u32 Codepoint;
    int IndexInGlyphs;
};

struct font_size
{
    u32 FontSizeEnumType;
    
    glyph* Glyphs;
    
    f32 PixelsPerMeter;
    f32 ScaleForPixelHeight;
};

struct font
{
    char UniqueName[64];
    u32 UniqueNameHash;
    
    image* Atlas;
    
    f32 Ascent;
    f32 Descent;
    f32 LineGap;
    f32 LineAdvance;
    
    float* KerningPairs;
    
    font_codepoint_slot_range* CodepointToSlot;
    font_slot_glyph_id* SlotsGlyphsIds;
    
    font_size* Sizes;
    int NumSizes;
    int NumGlyphs;
};

inline f32 GetScaleForPixelHeight(font_size* FontSize, f32 PixelHeight)
{
    f32 HeightOriginal = Global_FontSizes[FontSize->FontSizeEnumType];
    
    f32 Result = PixelHeight / HeightOriginal;
    
    return Result;
}

inline font_size* FindBestFontSizeForPixelHeight(font* Font, f32 PixelHeight)
{
    font_size* Result = &Font->Sizes[0];
    
    f32 MinAbsDiff = 999999.0f;
    
    for (int SizeIndex = 0;
         SizeIndex < Font->NumSizes;
         SizeIndex++)
    {
        font_size* Size = &Font->Sizes[SizeIndex];
        
#if 1
        //Finding closes by absolute difference
        f32 AbsDiff = FlowerAbs(Global_FontSizes[Size->FontSizeEnumType] - PixelHeight);
        
        if (AbsDiff < MinAbsDiff)
        {
            MinAbsDiff = AbsDiff;
            Result = Size;
        }
#else
        if (PixelHeight < Global_FontSizes[Size->FontSizeEnumType])
        {
            break;
        }
        else
        {
            Result = Size;
        }
#endif
    }
    
    return Result;
}

inline font_size* FindFontSizeForSizeType(font* Font, u32 FontSizeEnumType)
{
    font_size* Result = 0;
    
    Assert(Font->NumSizes > 0);
    
    for (int SizeIndex = 0;
         SizeIndex < Font->NumSizes;
         SizeIndex++)
    {
        font_size* Size = &Font->Sizes[SizeIndex];
        
        if (Size->FontSizeEnumType == FontSizeEnumType)
        {
            Result = Size;
            break;
        }
    }
    
    if (!Result)
    {
        Result = &Font->Sizes[0];
    }
    
    return Result;
}

inline int GetGlyphIndexByCodepoint(font* Font, int Codepoint)
{
    font_codepoint_slot_range* SlotRange = &Font->CodepointToSlot[Codepoint % FONT_MAPPING_SIZE];
    
    int Result = -1;
    
    for (int i = 0; i < SlotRange->Count; i++)
    {
        int IndexInSlotsGlyphsIds = SlotRange->StartIndexInSlotsGlyphsIds + i;
        
        font_slot_glyph_id* GlyphId = &Font->SlotsGlyphsIds[IndexInSlotsGlyphsIds];
        if (GlyphId->Codepoint == Codepoint)
        {
            Result = GlyphId->IndexInGlyphs;
            break;
        }
    }
    
    return Result;
}

struct model_node
{
    char Name[64];
    
    int StartInChildIndices;
    int StartInMeshIndices;
    
    int NumChildIndices;
    int NumMeshIndices;
    
    int ParentIndex;
};

struct model
{
    //mesh** Meshes;
    //material** Materials;
    
    std::vector<mesh*> Meshes;
    std::vector<material*> Materials;
    
    // NOTE(Dima): Nodes
    model_node* Nodes;
    m44* Node_ToParent;
    
    // NOTE(Dima): Bones
    m44* Bone_InvBindPose;
    int* Bone_NodeIndex;
    
    int* NodesMeshIndices;
    int* NodesChildIndices;
    
    int NumNodes;
    int NumBones;
    int NumNodesMeshIndices;
    int NumNodesChildIndices;
};

#endif //FLOWER_RENDER_PRIMITIVES_H
