#ifndef FLOWER_ASSET_H
#define FLOWER_ASSET_H

// TODO(Dima): We need special data deallocation (no deallocation actually) for 
// *** NodeAnimation assets
// *** FontSize assets

enum asset_type
{
    Asset_None,
    
    Asset_Image,
    Asset_Cubemap,
    Asset_Mesh,
    Asset_Material,
    Asset_Animation,
    Asset_NodeAnimation,
    Asset_Font,
    Asset_FontSize,
    Asset_GlyphStyle,
    Asset_Glyph,
    Asset_Model,
    
    Asset_Count,
};


// TODO(Dima): Maybe delete this here when asset loader implemented
// TODO(Dima): Because this file only contains information that is needed in the asset loader(packer).
#include "flower_asset_load.h"


#include "flower_asset_types_headers.h"
#include "flower_asset_sources.h"

union asset_data_pointer
{
    void* Ptr;
    
    image* Image;
    cubemap* Cubemap;
    mesh* Mesh;
    material* Material;
    animation* Animation;
    node_animation* NodeAnimation;
    font* Font;
    font_size* FontSize;
    glyph_style* GlyphStyle;
    glyph* Glyph;
    model* Model;
};

#define ASSET_GUID_SIZE 128

struct asset
{
    char GUID[ASSET_GUID_SIZE];
    
    u32 Type;
    u32 State;
    
    asset_header Header;
    asset_source Source;
    asset_data_pointer DataPtr;
    
    /*
     NOTE(Dima): If asset is supplemental it means that it is the part of other asset. 
    For example NodeAnimation or FontSize, they are parts of Animation and Font assets and used only
there.
    In such cases memory is not allocated for this asset directly, it's memory allocated when on
parent element creation (Actually it's memory IS the part of parent asset memory)
It means that the memory should not be freed for these assets.

Also it actually means that we'll never access these assets by GUID (It makes no sence).
    */
    b32 IsSupplemental;
};

struct asset_hashmap_entry
{
    u32 AssetGuidHash;
    
    asset_id AssetID;
};

struct asset_storage
{
    asset Assets[10000];
    int NumAssets;
    
#define ASSET_DEFAULT_COUNT_IN_TABLE 2048
    std::unordered_map<const char*, asset_id, string_fnv_key_hasher, string_comparator> GuidToID;
    
    b32 Initialized;
};

struct asset_pack
{
    asset_storage AssetStorage;
    
    b32 InUse;
    int IndexInPacks;
    
    char PackFileName[256];
    char PackBlobName[256];
};

struct asset_loading_context
{
#define MAX_ASSET_PACKS 64
    asset_pack Packs[MAX_ASSET_PACKS];
};

struct asset_system
{
    memory_arena* Arena;
    
    asset_storage AssetStorage;
    asset_loading_context LoadingCtx;
    
    image* VoxelAtlas;
    
    // NOTE(Dima): Assets
    mesh* Cube;
    mesh* Plane;
    
    cubemap* Sky;
    
    image* BoxTexture;
    image* PlaneTexture;
    image* Palette; 
    
#if 0    
    font TimesNewRoman;
    font LifeIsGoofy;
    font Arial;
#endif
    
    font* BerlinSans;
    font* LiberationMono;
    font* Dimbo;
    
    material PaletteMaterial;
    material GroundMaterial;
};

#endif //FLOWER_ASSET_H
