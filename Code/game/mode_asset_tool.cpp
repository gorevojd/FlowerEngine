#include "asset_tool.h"
#include "asset_tool.cpp"


#if 0
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
#endif

INTERNAL_FUNCTION 
b32 ButtonAt(font* Font,
             char* ButtonText,
             rc2 PackRect)
{
    b32 Result = false;
    
    v4 ColorBack = ColorGray(0.05f);
    v4 ColorText = ColorGray(0.95f);
    
    ui_element* Element = UIBeginElement(ButtonText, UIElement_Static);
    
    // NOTE(Dima): Processing global interaction
    ui_interaction Interaction = CreateInteraction(Element, InteractionPriority_Avg);
    
    ProcessMouseKeyInteractionInRect(&Interaction, 
                                     KeyMouse_Left, 
                                     PackRect);
    
    
    if (Interaction.WasHotInInteraction)
    {
        ColorBack = ColorGray(0.95f);
        ColorText = ColorGray(0.05f);
        
        if (Interaction.WasActiveInInteraction)
        {
            Result = true;
        }
    }
    
    // NOTE(Dima): Pusing rect and it's outline 
    PushRect(Global_RenderCommands->DEBUG_Rects2D_Window,
             PackRect,
             ColorBack);
    PushRectOutline(Global_RenderCommands->DEBUG_Rects2D_Window,
                    PackRect,
                    3.0f,
                    ColorBlack());
    
    // NOTE(Dima): Printing text
    PrintTextWithFontCenteredInRect(Font,
                                    Element->DisplayName,
                                    PackRect,
                                    ColorText);
    
    UIEndElement(UIElement_Static);
    
    return Result;
}

INTERNAL_FUNCTION void DisplayAssetPacksScreen(asset_tool* Tool)
{
    asset_id DimboID = GetAssetID("Font_Dimbo");
    font* Font = G_GetAssetDataByID(DimboID, font);
    
    iv2 WndDims = G_GetCurrentWindowDim();
    
    if (BeginLayout("AssetPacks"))
    {
        
        v2 StartAt = V2(50.0f, 200.0f);
        v2 PrintAt = StartAt;
        v2 ButtonDim = V2(400.0f, 100.0f);
        v2 Spacing = V2(50);
        
        for (int PackIndex = 0;
             PackIndex < Tool->NumPacksInUse;
             PackIndex++)
        {
            asset_pack* Pack = &Tool->Packs[PackIndex];
            
            rc2 PackRect = RectMinDim(PrintAt, ButtonDim);
            
            if (PackRect.Max.x > WndDims.x)
            {
                PrintAt.x = StartAt.x;
                PrintAt.y = PackRect.Max.y + Spacing.y;
                
                PackRect = RectMinDim(PrintAt, ButtonDim);
            }
            
            b32 Clicked = ButtonAt(Font, 
                                   Pack->Name, 
                                   PackRect);
            
            PrintAt.x = PackRect.Max.x + Spacing.x;
        }
        
        EndLayout();
    }
    
}


SCENE_INIT(AssetTool)
{
    asset_tool* Tool = GetSceneState(asset_tool);
    
    UseAssetPack(Tool, "Cars");
    UseAssetPack(Tool, "Fonts");
    UseAssetPack(Tool, "Animals");
    UseAssetPack(Tool, "Minecraft");
    
    
    //AddCommonAssets(&Global_Assets->AssetStorage);
}

SCENE_UPDATE(AssetTool)
{
    asset_tool* Tool = GetSceneState(asset_tool);
    
    PushClear(ColorRed().rgb);
    
    DisplayAssetPacksScreen(Tool);
}