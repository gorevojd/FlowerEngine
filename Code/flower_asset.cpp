#include "flower_asset_shared.cpp"

// TODO(Dima): This should be included in asset tool at some point in time
#include "asset_tool_adding.cpp"

#define G_GetAssetDataByID(id, type) (type*)GetAssetDataByID_(GetGlobalAssetStorage(), id)

INTERNAL_FUNCTION inline 
asset_id GetAssetID(const char* GUID)
{
    asset_id Result = GetAssetID(&Global_Assets->AssetStorage, GUID);
    
    return Result;
}

INTERNAL_FUNCTION inline 
asset_storage* GetGlobalAssetStorage()
{
    asset_storage* Result = &Global_Assets->AssetStorage;
    
    return Result;
}

INTERNAL_FUNCTION 
void AddBear(asset_storage* Storage)
{
    loading_params Params = LoadingParams_Model();
    Params.Model.DefaultScale = 0.01f;
    Params.Model.FixInvalidRotation = true;
    
    asset_id ModelID = AddAssetModel(Storage, 
                                     "Model_Bear", 
                                     "../Data/ForGame/Common/ForestAnimals/Bear/bear.FBX", 
                                     Params);
    
    // NOTE(Dima): Loading animations
    AddAssetAnimationFirst(Storage,
                           "Anim_Bear_Idle",
                           "../Data/ForGame/Common/ForestAnimals/Bear/animations/Idle.FBX");
    
    AddAssetAnimationFirst(Storage,
                           "Anim_Bear_Success",
                           "../Data/ForGame/Common/ForestAnimals/Bear/animations/Success.FBX");
    
    
    // NOTE(Dima): Loading textures
    asset_id ImgDiffuseID = AddAssetImage(Storage, 
                                          "Image_Bear_Diffuse",
                                          "../Data/ForGame/Common/ForestAnimals/Bear/Textures/Bear.tga");
    asset_id ImgNormalsID = AddAssetImage(Storage,
                                          "Image_Bear_Normal",
                                          "../Data/ForGame/Common/ForestAnimals/Bear/Textures/Bear Normals.tga");
    asset_id ImgEyesID = AddAssetImage(Storage,
                                       "Image_Bear_EyesDiffuse",
                                       "../Data/ForGame/Common/ForestAnimals/Bear/Textures/Eye Bear.tga");
    asset_id ImgEyesShineID = AddAssetImage(Storage,
                                            "Image_Bear_EyesShine",
                                            "../Data/ForGame/Common/ForestAnimals/Bear/Textures/Eye Shine Bear.tga");
    
    // NOTE(Dima): Setting main material
    asset_id MatBearID = AddAssetMaterial(Storage,
                                          "Material_Bear",
                                          "BearMain",
                                          Material_SpecularDiffuse);
    SetMaterialTexture(Storage,
                       MatBearID,
                       MatTex_SpecularDiffuse_Diffuse,
                       ImgDiffuseID);
    SetMaterialTexture(Storage,
                       MatBearID,
                       MatTex_SpecularDiffuse_Normal,
                       ImgNormalsID);
    
    // NOTE(Dima): Setting eyes material
    asset_id MatBearEyesID = AddAssetMaterial(Storage,
                                              "Material_BearEyes",
                                              "BearEyes",
                                              Material_SpecularDiffuse);
    SetMaterialTexture(Storage,
                       MatBearEyesID,
                       MatTex_SpecularDiffuse_Diffuse,
                       ImgEyesID);
    
    
    // NOTE(Dima): Setting eyes shine material
    asset_id MatBearEyesShineID = AddAssetMaterial(Storage,
                                                   "Material_BearEyesShine",
                                                   "BearEyesShine",
                                                   Material_SpecularDiffuse);
    SetMaterialTexture(Storage,
                       MatBearEyesShineID,
                       MatTex_SpecularDiffuse_Diffuse,
                       ImgEyesShineID);
    
    
    // NOTE(Dima): Setting model's materials
    SetModelMaterial(Storage, 
                     ModelID,
                     0,
                     MatBearID);
    
    SetModelMaterial(Storage,
                     ModelID,
                     1,
                     MatBearEyesID);
    
    SetModelMaterial(Storage,
                     ModelID,
                     2,
                     MatBearEyesShineID);
}

INTERNAL_FUNCTION 
void AddFox(asset_storage* Storage)
{
    loading_params Params = LoadingParams_Model();
    Params.Model.DefaultScale = 0.01f;
    Params.Model.FixInvalidRotation = true;
    
    asset_id ModelID = AddAssetModel(Storage, 
                                     "Model_Fox", 
                                     "../Data/ForGame/Common/ForestAnimals/Fox/Fox.FBX", 
                                     Params);
    
    // NOTE(Dima): Loading animations
    AddAssetAnimationFirst(Storage,
                           "Anim_Fox_Talk",
                           "../Data/ForGame/Common/ForestAnimals/Fox/animations/Talk.FBX");
    
    
    
    // NOTE(Dima): Loading textures
    asset_id ImgDiffuseID = AddAssetImage(Storage, 
                                          "Image_Fox_Diffuse",
                                          "../Data/ForGame/Common/ForestAnimals/Fox/Textures/Fox.tga");
    asset_id ImgNormalsID = AddAssetImage(Storage,
                                          "Image_Fox_Normal",
                                          "../Data/ForGame/Common/ForestAnimals/Fox/Textures/Fox Normals.tga");
    asset_id ImgEyesID = AddAssetImage(Storage,
                                       "Image_Fox_EyesDiffuse",
                                       "../Data/ForGame/Common/ForestAnimals/Fox/Textures/Eye Green.tga");
    asset_id ImgEyesShineID = AddAssetImage(Storage,
                                            "Image_Fox_EyesShine",
                                            "../Data/ForGame/Common/ForestAnimals/Fox/Textures/Eye Shine.tga");
    
    // NOTE(Dima): Setting main material
    asset_id MatID = AddAssetMaterial(Storage,
                                      "Material_Fox",
                                      "FoxMain",
                                      Material_SpecularDiffuse);
    SetMaterialTexture(Storage,
                       MatID,
                       MatTex_SpecularDiffuse_Diffuse,
                       ImgDiffuseID);
    SetMaterialTexture(Storage,
                       MatID,
                       MatTex_SpecularDiffuse_Normal,
                       ImgNormalsID);
    
    // NOTE(Dima): Setting eyes material
    asset_id MatEyesID = AddAssetMaterial(Storage,
                                          "Material_FoxEyes",
                                          "FoxEyes",
                                          Material_SpecularDiffuse);
    SetMaterialTexture(Storage,
                       MatEyesID,
                       MatTex_SpecularDiffuse_Diffuse,
                       ImgEyesID);
    
    
    // NOTE(Dima): Setting eyes shine material
    asset_id MatEyesShineID = AddAssetMaterial(Storage,
                                               "Material_FoxEyesShine",
                                               "FoxEyesShine",
                                               Material_SpecularDiffuse);
    SetMaterialTexture(Storage,
                       MatEyesShineID,
                       MatTex_SpecularDiffuse_Diffuse,
                       ImgEyesShineID);
    
    
    // NOTE(Dima): Setting model's materials
    SetModelMaterial(Storage, 
                     ModelID,
                     0,
                     MatID);
    
    SetModelMaterial(Storage,
                     ModelID,
                     1,
                     MatEyesID);
    
    SetModelMaterial(Storage,
                     ModelID,
                     2,
                     MatEyesShineID);
}

INTERNAL_FUNCTION
void AddAnimals(asset_storage* Storage)
{
    AddBear(Storage);
    AddFox(Storage);
}

INTERNAL_FUNCTION void AddCars(asset_storage* Storage)
{
    loading_params PaletteParams = LoadingParams_Image();
    PaletteParams.Image.FilteringIsClosest = true;
    asset_id PaletteID = AddAssetImage(Storage, 
                                       "Image_Palette",
                                       "../Data/ForGame/Common/MyPallette.png",
                                       PaletteParams);
    
    asset_id PaletteMaterialID = AddAssetMaterial(Storage,
                                                  "Material_Palette",
                                                  "PaletteMaterial",
                                                  Material_SpecularDiffuse);
    SetMaterialTexture(Storage, 
                       PaletteMaterialID, 
                       MatTex_SpecularDiffuse_Diffuse,
                       PaletteID);
    
    // NOTE(Dima): Adding cars models
    asset_id CarModelIDs[] = 
    {
        
        AddAssetModel(Storage,
                      "Model_Car_Supra",
                      "../Data/ForGame/Common/Cars/Supra.FBX"),
        
        AddAssetModel(Storage,
                      "Model_Car_Mustang",
                      "../Data/ForGame/Common/Cars/mustanggt6gen.fbx"),
        
        AddAssetModel(Storage,
                      "Model_Car_NissanGTR",
                      "../Data/ForGame/Common/Cars/NissanGTR.fbx"),
        
        AddAssetModel(Storage,
                      "Model_Car_Golf2",
                      "../Data/ForGame/Common/Cars/golf2.fbx"),
        
        AddAssetModel(Storage,
                      "Model_Car_LamborginiAventador",
                      "../Data/ForGame/Common/Cars/aventador.fbx"),
        
        AddAssetModel(Storage,
                      "Model_Car_HondaCivic",
                      "../Data/ForGame/Common/Cars/HondaCivic.fbx"),
        
        AddAssetModel(Storage,
                      "Model_Car_NissanJuke",
                      "../Data/ForGame/Common/Cars/NissanJuke.fbx"),
        
        AddAssetModel(Storage,
                      "Model_Car_SchoolBus",
                      "../Data/ForGame/Common/Cars/SchoolBus.fbx"),
        
        AddAssetModel(Storage,
                      "Model_Car_Zhigul",
                      "../Data/ForGame/Common/Cars/vaz2107.fbx"),
    };
    
    for(int CarIndex = 0;
        CarIndex < ArrLen(CarModelIDs);
        CarIndex++)
    {
        asset_id CarAssetID = CarModelIDs[CarIndex];
        
        SetModelMaterial(Storage,
                         CarAssetID,
                         0,
                         PaletteMaterialID);
    }
}


INTERNAL_FUNCTION 
void AddFonts(asset_storage* Storage)
{
    AddAssetFont(Storage, 
                 "Font_BerlinSans", 
                 "../Data/Fonts/BerlinSans.ttf");
    
    AddAssetFont(Storage, 
                 "Font_LiberationMono", 
                 "../Data/Fonts/liberation-mono.ttf");
    
    AddAssetFont(Storage,
                 "Font_Dimbo",
                 "../Data/Fonts/Dimbo Regular.ttf");
}

INTERNAL_FUNCTION 
void AddCommonAssets(asset_storage* Storage)
{
    AddCars(Storage);
    AddFonts(Storage);
    AddAnimals(Storage);
    
    // NOTE(Dima): Loading default images
    asset_id BoxDiffuseID = AddAssetImage(Storage, 
                                          "Image_BoxDiffuse", 
                                          "../Data/Textures/container_diffuse.png");
    
    asset_id BoxSpecularID = AddAssetImage(Storage,
                                           "Image_BoxSpecular",
                                           "../Data/Textures/container_specular.png");
    
    asset_id FlowerPatternID = AddAssetImage(Storage, 
                                             "Image_PlaneTexture", 
                                             "../Data/Textures/PixarTextures/png/fabric/Flower_pattern_pxr128.png");
    
    
    // NOTE(Dima): Loading default meshes
    mesh* UnitCube = MakeUnitCube();
    mesh* Plane = MakePlane();
    
    AddAssetMesh(Storage,
                 "Mesh_Cube", 
                 UnitCube);
    
    AddAssetMesh(Storage, 
                 "Mesh_Plane", 
                 Plane);
    
    // NOTE(Dima): Adding default plane material
    asset_id PlaneMatID = AddAssetMaterial(Storage,
                                           "Material_DefaultPlane",
                                           "DefaultPlaneMaterial",
                                           Material_SpecularDiffuse);
    SetMaterialTexture(Storage,
                       PlaneMatID,
                       MatTex_SpecularDiffuse_Diffuse,
                       FlowerPatternID);
    
    // NOTE(Dima): Adding default cube material
    asset_id CubeMatID = AddAssetMaterial(Storage,
                                          "Material_DefaultCube",
                                          "DefaultCubeMaterial",
                                          Material_SpecularDiffuse);
    
    SetMaterialTexture(Storage,
                       CubeMatID,
                       MatTex_SpecularDiffuse_Diffuse,
                       BoxDiffuseID);
    
    SetMaterialTexture(Storage,
                       CubeMatID,
                       MatTex_SpecularDiffuse_Specular,
                       BoxSpecularID);
    
    // NOTE(Dima): Loading default cubemaps
    AddAssetCubemap(Storage,
                    "Cubemap_Default",
                    "../Data/Textures/Cubemaps/skybox/right.jpg",
                    "../Data/Textures/Cubemaps/skybox/left.jpg",
                    "../Data/Textures/Cubemaps/skybox/front.jpg",
                    "../Data/Textures/Cubemaps/skybox/back.jpg",
                    "../Data/Textures/Cubemaps/skybox/top.jpg",
                    "../Data/Textures/Cubemaps/skybox/bottom.jpg");
    
    AddAssetCubemap(Storage, "Cubemap_DefaultPink", 
                    "../Data/Textures/Cubemaps/Pink/left.png",
                    "../Data/Textures/Cubemaps/Pink/right.png",
                    "../Data/Textures/Cubemaps/Pink/front.png",
                    "../Data/Textures/Cubemaps/Pink/back.png",
                    "../Data/Textures/Cubemaps/Pink/up.png",
                    "../Data/Textures/Cubemaps/Pink/down.png");
    
    // NOTE(Dima): Adding minecraft textures
    
    loading_params VoxelAtlasParams = LoadingParams_Image();
    VoxelAtlasParams.Image.FilteringIsClosest = true;
    AddAssetImage(Storage, 
                  "Image_VoxelAtlasOriginal", 
                  "../Data/Textures/minc_atlas1.jpg",
                  VoxelAtlasParams);
    
    AddAssetImage(Storage, 
                  "Image_VoxelAtlasCool", 
                  "../Data/Textures/minc_atlas2.png",
                  VoxelAtlasParams);
}

INTERNAL_FUNCTION 
void StartAssetLoading(asset_storage* Storage, 
                       asset_id AssetID,
                       b32 Immediate)
{
    
}

INTERNAL_FUNCTION void InitAssetSystem(memory_arena* Arena)
{
    Global_Assets = PushStruct(Arena, asset_system);
    Global_Assets->Arena = Arena;
    
    asset_system* A = Global_Assets;
    
    InitAssetStorage(&A->AssetStorage);
    
    {
        //asset_pack* Pack = UseAssetPack(&Global_Assets->LoadingCtx, "common");
        //asset_storage* Storage = &Pack->AssetStorage;
        
        AddCommonAssets(&Global_Assets->AssetStorage);
        
#if 0
        
        WriteAssetPackToFile(Pack);
#endif
        
    }
}

