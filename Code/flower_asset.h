#ifndef FLOWER_ASSET_H
#define FLOWER_ASSET_H

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
    Asset_Model,
    
    Asset_Count,
};


// TODO(Dima): Maybe delete this here when asset loader implemented
// TODO(Dima): Because this file only contains information that is needed in the asset loader(packer).
#include "flower_asset_load.h"


#include "flower_asset_types.h"
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
    
    memory_arena Arena;
    
#define ASSET_STORAGE_HASHMAP_SIZE 512
    hashmap<asset_hashmap_entry, ASSET_STORAGE_HASHMAP_SIZE> GuidToID;
    
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
#define MAX_ASSET_PACKS 32
    asset_pack Packs[MAX_ASSET_PACKS];
};

struct asset_system
{
    memory_arena* Arena;
    
    asset_storage AssetStorage;
    asset_loading_context LoadingCtx;
    
    image* VoxelAtlas;
    
    // NOTE(Dima): Assets
    mesh Cube;
    mesh Plane;
    
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
    
    image* BearDiffuse;
    image* BearNormal;
    image* BearEyesDiffuse;
    image* BearEyesShine;
    
    image* FoxDiffuse;
    image* FoxNormal;
    image* FoxEyesDiffuse;
    image* FoxEyesShine;
    
    model* Bear;
    model* Fox;
    model* Supra;
    model* Mustang;
    model* NissanGTR;
    model* Golf2;
    model* Aventador;
    
    animation* BearSuccess;
    animation* BearIdle;
    animation* FoxTalk;
    
    material BearMaterial;
    material BearEyesMaterial;
    material BearEyesShineMaterial;
    
    material FoxMaterial;
    material FoxEyesMaterial;
    material FoxEyesShineMaterial;
    
    material PaletteMaterial;
    material GroundMaterial;
};

#endif //FLOWER_ASSET_H
