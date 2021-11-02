#ifndef FLOWER_ASSET_H
#define FLOWER_ASSET_H

// TODO(Dima): Text segment in asset file to hold all names of various assets

#include <unordered_map>

enum asset_type
{
    Asset_None,
    
    Asset_Font,
    Asset_Image,
    Asset_Mesh,
    Asset_Animation,
    Asset_NodeAnimation,
    Asset_Material,
    Asset_Model,
    Asset_Skybox,
    
    Asset_Count,
};

typedef u32 asset_id;

struct asset_system
{
    memory_arena* Arena;
    
    std::unordered_map<std::string, asset_id>* NameToAssetID;
    //asset Assets[2048];
    int NumAssets;
    
    image FontsAtlas;
    v2 FontAtlasAtP;
    int FontAtlasMaxRowY;
    
    image* VoxelAtlas;
    
    // NOTE(Dima): Assets
    mesh Cube;
    mesh Plane;
    
    cubemap Sky;
    
    image* BoxTexture;
    image* PlaneTexture;
    image* Palette; 
    
#if 0    
    font TimesNewRoman;
    font LifeIsGoofy;
    font Arial;
#endif
    
    font BerlinSans;
    font LiberationMono;
    
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
    
    animation BearSuccess;
    animation BearIdle;
    animation FoxTalk;
    
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
