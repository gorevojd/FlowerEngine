#ifndef FLOWER_ASSET_TYPES_HEADERS_H
#define FLOWER_ASSET_TYPES_HEADERS_H

#define ASSET_HEADER_MAX_SIZE 512

#define ASSET_ASSERT_HEADER(header_type) \
static_assert(sizeof(header_type) <= ASSET_HEADER_MAX_SIZE, \
"Size of header should be smaller than MAX size")


struct asset_header_image
{
    int Width;
    int Height;
    b32 FilteringIsNearest;
    
    f32 AlignX;
    f32 AlignY;
    
    u32 BlobDataOffset;
};
ASSET_ASSERT_HEADER(asset_header_image);


struct asset_header_cubemap
{
    asset_id Left;
    asset_id Right;
    asset_id Front;
    asset_id Back;
    asset_id Up;
    asset_id Down;
};
ASSET_ASSERT_HEADER(asset_header_cubemap);


struct asset_header_mesh
{
    int VertexCount;
    int IndexCount;
    b32 IsSkinned;
    int MaterialIndexInModel;
    
    u32 BlobOffsetP;
    u32 BlobOffsetUV;
    u32 BlobOffsetN;
    u32 BlobOffsetC;
    u32 BlobOffsetBoneWeights;
    u32 BlobOffsetBoneIDs;
};
ASSET_ASSERT_HEADER(asset_header_mesh);


struct asset_header_material
{
    char Name[64];
    
    u32 Type;
    
    asset_id TextureIDs[MAX_MATERIAL_TEXTURES];
};
ASSET_ASSERT_HEADER(asset_header_material);


struct asset_header_node_animation
{
    u32 BlobOffset_PosKeys;
    u32 BlobOffset_RotKeys;
    u32 BlobOffset_ScaKeys;
    
    u32 BlobOffset_PosTimes;
    u32 BlobOffset_RotTimes;
    u32 BlobOffset_ScaTimes;
    
    int NumPos;
    int NumRot;
    int NumSca;
    
    int NodeIndex;
};
ASSET_ASSERT_HEADER(asset_header_node_animation);


struct asset_header_animation
{
    char Name[64];
    
    int NumNodeAnims;
    asset_id FirstNodeAnimID;
    
    f32 DurationTicks;
    f32 TicksPerSecond;
    
    u32 OutsideBehaviour;
};
ASSET_ASSERT_HEADER(asset_header_animation);


struct asset_header_font_size
{
    u32 FontSizeEnumType;
    
    asset_id FirstGlyphId;
    
    f32 PixelsPerMeter;
    f32 ScaleForPixelHeight;
};
ASSET_ASSERT_HEADER(asset_header_font_size);


struct asset_header_glyph_style
{
    asset_id ImageID;
    
    u32 GlyphStyleType;
    int ImageWidth;
    int ImageHeight;
    
    f32 MinUV_x;
    f32 MinUV_y;
    
    f32 MaxUV_x;
    f32 MaxUV_y;
};
ASSET_ASSERT_HEADER(asset_header_glyph_style);


struct asset_header_glyph
{
    asset_id FirstStyleID;
    int NumStyles;
    
    u32 Codepoint;
    
    f32 Advance;
    f32 LeftBearing;
    
    f32 XOffset;
    f32 YOffset;
};
ASSET_ASSERT_HEADER(asset_header_glyph);


struct asset_header_font
{
    u32 BlobOffset_SizesIds;
    u32 BlobOffset_Mapping;
    u32 BlobOffset_SlotsGlyphsIds;
    u32 BlobOffset_KerningPairs;
    
    int NumSizes;
    int NumGlyphs;
    
    f32 Ascent;
    f32 Descent;
    f32 LineGap;
    f32 LineAdvance;
    u32 UniqueNameHash;
    char UniqueName[64];
    
    asset_id AtlasImageID;
    asset_id FirstFontSizeID;
};
ASSET_ASSERT_HEADER(asset_header_font);


struct asset_header_model
{
    u32 BlobOffset_MeshIds;
    u32 BlobOffset_MaterialIds;
    u32 BlobOffset_Nodes;
    u32 BlobOffset_Node_ToParent;
    u32 BlobOffset_Bone_InvBindPose;
    u32 BlobOffset_Bone_NodeIndex;
    u32 BlobOffset_NodesMeshIndices;
    u32 BlobOffset_NodesChildIndices;
    
    int NumMeshes;
    int NumMaterials;
    int NumNodes;
    int NumBones;
    int NumNodesMeshIndices;
    int NumNodesChildIndices;
    
    std::vector<asset_id> MeshIDs;
    std::vector<asset_id> MaterialIDs;
};
ASSET_ASSERT_HEADER(asset_header_model);


// NOTE(Dima): Asset header
struct asset_header
{
    u8 HeaderBytes[ASSET_HEADER_MAX_SIZE];
    
    union
    {
        void* Ptr;
        
        asset_header_image* Image;
        asset_header_cubemap* Cubemap;
        asset_header_mesh* Mesh;
        asset_header_material* Material;
        asset_header_animation* Animation;
        asset_header_node_animation* NodeAnimation;
        asset_header_font* Font;
        asset_header_font_size* FontSize;
        asset_header_glyph* Glyph;
        asset_header_glyph_style* GlyphStyle;
        asset_header_model* Model;
    };
};

#endif //FLOWER_ASSET_TYPES_H
