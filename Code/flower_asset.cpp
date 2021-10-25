#include "flower_asset_load.cpp"

INTERNAL_FUNCTION void AddGlyphToAtlas(font* Font, int GlyphIndex)
{
    image* Dst = &Global_Assets->FontsAtlas;
    
    glyph* Glyph = &Font->Glyphs[GlyphIndex];
    
    int DstSize = Global_Assets->FontsAtlas.Width;
    f32 OneOverSize = 1.0f / DstSize;
    
    for(int StyleIndex = 0; 
        StyleIndex < FontStyle_Count;
        StyleIndex++)
    {
        glyph_style* Style = &Glyph->Styles[StyleIndex];
        
        // NOTE(Dima): Getting image 
        int ImageIndex = Style->ImageIndex;
        if(ImageIndex != -1)
        {
            image* Src = Font->GlyphImages[ImageIndex];
            int SrcW = Src->Width;
            int SrcH = Src->Height;
            
            int DstPx = std::ceil(Global_Assets->FontAtlasAtP.x);
            int DstPy = std::ceil(Global_Assets->FontAtlasAtP.y);
            
            Style->MinUV = V2(DstPx, DstPy) * OneOverSize;
            Style->MaxUV = V2(DstPx + SrcW, DstPy + SrcH) * OneOverSize;
            
            if(DstPx + SrcW >= DstSize)
            {
                DstPx = 0;
                DstPy = Global_Assets->FontAtlasMaxRowY;
            }
            
            Assert(DstPy + SrcH < DstSize);
            
            // NOTE(Dima): Copy pixels
            for(int y = 0; y < SrcH; y++)
            {
                for(int x = 0; x < SrcW; x++)
                {
                    int DstPixelY = DstPy + y;
                    int DstPixelX = DstPx + x;
                    
                    u32* DstPixel = (u32*)Dst->Pixels + DstPixelY * DstSize + DstPixelX;
                    u32* SrcPixel = (u32*)Src->Pixels + y * SrcW + x;
                    
                    *DstPixel = *SrcPixel;
                }
            }
            
            Global_Assets->FontAtlasAtP = V2(DstPx + SrcW, DstPy);
            Global_Assets->FontAtlasMaxRowY = std::max(Global_Assets->FontAtlasMaxRowY, DstPy + SrcH);
        }
    }
}

INTERNAL_FUNCTION void AddFontToAtlas(font* Font)
{
    for(int GlyphIndex = 0;
        GlyphIndex < Font->GlyphCount;
        GlyphIndex++)
    {
        glyph* Glyph = &Font->Glyphs[GlyphIndex];
        
        AddGlyphToAtlas(Font, GlyphIndex);
    }
}

INTERNAL_FUNCTION char* GenerateSpecialGUID(char* Buf, 
                                            int BufSize,
                                            const char* BaseGUID,
                                            const char* SpecialStr)
{
    char* StringsToConcat[] = 
    {
        (char*)BaseGUID,
        "_",
        (char*)SpecialStr
    };
    
    ConcatBunchOfStrings(Buf, BufSize, 
                         StringsToConcat,
                         ARC(StringsToConcat));
    
    return (Buf);
}

#if 0
struct asset_pack
{
    char PackFileName[256];
    char PackBlobName[256];
    
    vector<asset> Assets;
};

INTERNAL_FUNCTION asset_pack* CreateAssetPack(char* Name)
{
    asset_pack* Pack = new asset_pack;
    
    ClearString(Pack->PackFileName, ArrayCount(Pack->PackFileName));
    AppendToString(Pack->PackFileName, ArrayCount(Pack->PackFileName), Name);
    AppendToString(Pack->PackFileName, ArrayCount(Pack->PackFileName), ".json");
    
    ClearString(Pack->PackFileName, ArrayCount(Pack->PackFileName));
    AppendToString(Pack->PackBlobName, ArrayCount(Pack->PackBlobName), Name);
    AppendToString(Pack->PackBlobName, ArrayCount(Pack->PackBlobName), ".blob");
    
    return Pack;
}

INTERNAL_FUNCTION asset_id AddAssetInternal(asset_pack* Pack, 
                                            const char* GUID, 
                                            u32 Type, 
                                            void* Ptr)
{
    asset NewAsset;
    
    CopyStringsSafe(NewAsset.GUID, ASSET_GUID_SIZE, (char*)GUID);
    NewAsset.Type = Type;
    NewAsset.Ptr = Ptr;
    
    // NOTE(Dima): Inserting to assets array
    Pack->Assets.push_back(NewAsset);
    
    return(NewAssetID);
}

INTERNAL_FUNCTION asset_id AddAssetBitmap(const char* GUID, 
                                          const char* Path,
                                          const loading_params& Params)
{
    
}

INTERNAL_FUNCTION asset_id AddAssetSkybox(const char* GUID,
                                          asset_id LeftID,
                                          asset_id RightID,
                                          asset_id FrontID,
                                          asset_id BackID,
                                          asset_id RightID,
                                          asset_id UpID,
                                          asset_id DownID,
                                          const loading_params& Params)
{
    
}

INTERNAL_FUNCTION asset_id AddAssetSkybox(const char* GUID,
                                          const char* LeftPath,
                                          const char* RightPath,
                                          const char* FrontPath,
                                          const char* BackPath,
                                          const char* UpPath,
                                          const char* DownPath,
                                          const loading_params& Params)
{
    char GuidBuf[128];
    
    GenerateSpecialGUID(GuidBuf, ARC(GuidBuf), GUID, "Left");
    asset_id LeftID = AddAssetBitmap(GuidBuf, LeftPath);
    
    GenerateSpecialGUID(GuidBuf, ARC(GuidBuf), GUID, "Right");
    asset_id RightID = AddAssetBitmap(GuidBuf, RightPath);
    
    GenerateSpecialGUID(GuidBuf, ARC(GuidBuf), GUID, "Front");
    asset_id FrontID = AddAssetBitmap(GuidBuf, FrontPath);
    
    GenerateSpecialGUID(GuidBuf, ARC(GuidBuf), GUID, "Back");
    asset_id BackID = AddAssetBitmap(GuidBuf, BackPath);
    
    GenerateSpecialGUID(GuidBuf, ARC(GuidBuf), GUID, "Up");
    asset_id UpID = AddAssetBitmap(GuidBuf, UpPath);
    
    GenerateSpecialGUID(GuidBuf, ARC(GuidBuf), GUID, "Down");
    asset_id DownID = AddAssetBitmap(GuidBuf, DownPath);
    
    asset_id Result = AddAssetSkybox(GUID,
                                     LeftID, RightID,
                                     FrontID, BackID,
                                     UpID, DownID,
                                     Params);
    
    return Result;
}
#endif

INTERNAL_FUNCTION void InitAssetSystem(memory_arena* Arena)
{
    Global_Assets = PushStruct(Arena, asset_system);
    Global_Assets->Arena = Arena;
    
    asset_system* A = Global_Assets;
    
    Global_Assets->NameToAssetID = PushNew<std::unordered_map<std::string, asset_id>>(Arena);
    
    // NOTE(Dima): Font atlas initializing
    int FontAtlasSize = 2048;
    void* FontsAtlasMem = calloc(FontAtlasSize * FontAtlasSize * sizeof(u32), 1);
    AllocateImageInternal(&Global_Assets->FontsAtlas,
                          FontAtlasSize,
                          FontAtlasSize,
                          FontsAtlasMem);
    
#if 1
    loading_params VoxelAtlasParams = DefaultLoadingParams();
    VoxelAtlasParams.Image_FilteringIsClosest = true;
    A->VoxelAtlas = LoadImageFile("../Data/Textures/minc_atlas2.png", VoxelAtlasParams);
    //A->VoxelAtlas = LoadImageFile("../Data/Textures/minc_atlas1.jpg", VoxelAtlasParams);
    
#if 0
    // NOTE(Dima): Loading fonts
    A->TimesNewRoman = LoadFontFile("C:/Windows/Fonts/times.ttf");
    A->LifeIsGoofy = LoadFontFile("../Data/Fonts/Life is goofy.ttf");
    A->Arial = LoadFontFile("c:/windows/fonts/arial.ttf");
#endif
    
    loading_params BerlinSansParams = DefaultLoadingParams();
    BerlinSansParams.Font_PixelHeight = 60;
    A->BerlinSans = LoadFontFile("../Data/Fonts/BerlinSans.ttf", BerlinSansParams);
    
    loading_params LibMonoParams = DefaultLoadingParams();
    LibMonoParams.Font_PixelHeight = 24;
    A->LiberationMono = LoadFontFile("../Data/Fonts/liberation-mono.ttf", LibMonoParams);
    
#if 0    
    AddFontToAtlas(&A->TimesNewRoman);
    AddFontToAtlas(&A->LifeIsGoofy);
    AddFontToAtlas(&A->Arial);
#endif
    AddFontToAtlas(&A->BerlinSans);
    AddFontToAtlas(&A->LiberationMono);
    
    // NOTE(Dima): Loading assets
    A->Cube = MakeUnitCube();
    A->Plane = MakePlane();
    
    
#if 0    
    A->Sky = LoadCubemap(
                         "../Data/Textures/Cubemaps/skybox/right.jpg",
                         "../Data/Textures/Cubemaps/skybox/left.jpg",
                         "../Data/Textures/Cubemaps/skybox/front.jpg",
                         "../Data/Textures/Cubemaps/skybox/back.jpg",
                         "../Data/Textures/Cubemaps/skybox/top.jpg",
                         "../Data/Textures/Cubemaps/skybox/bottom.jpg");
#else
    A->Sky = LoadCubemap(
                         "../Data/Textures/Cubemaps/Pink/left.png",
                         "../Data/Textures/Cubemaps/Pink/right.png",
                         "../Data/Textures/Cubemaps/Pink/front.png",
                         "../Data/Textures/Cubemaps/Pink/back.png",
                         "../Data/Textures/Cubemaps/Pink/up.png",
                         "../Data/Textures/Cubemaps/Pink/down.png");
#endif
    
    A->BoxTexture = LoadImageFile("../Data/Textures/container_diffuse.png");
    A->PlaneTexture = LoadImageFile("E:/Media/PixarTextures/png/ground/Red_gravel_pxr128.png");
    loading_params PaletteParams = DefaultLoadingParams();
    PaletteParams.Image_FilteringIsClosest = true;
    A->Palette = LoadImageFile("E:/Development/Modeling/Pallette/MyPallette.png", PaletteParams);
    
    A->BearDiffuse = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Bear/Bear.tga");
    A->BearNormal = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Bear/Bear Normals.tga");
    A->BearEyesDiffuse = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Bear/Eye Bear.tga");
    A->BearEyesShine = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Bear/Eye Shine Bear.tga");
    
    A->FoxDiffuse = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Fox/Fox.tga");
    A->FoxNormal = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Fox/Fox Normals.tga");
    A->FoxEyesDiffuse = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Fox/Eye Green.tga");
    A->FoxEyesShine = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Fox/Eye Shine.tga");
    
    loading_params BearParams = DefaultLoadingParams();
    BearParams.Model_DefaultScale = 0.01f;
    BearParams.Model_FixInvalidRotation = true;
    
    loading_params FoxParams = BearParams;
    
    A->Bear = LoadModel("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Bear/bear.FBX", BearParams);
    A->Fox = LoadModel("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Fox/Fox.FBX", FoxParams);
    A->Supra = LoadModel("E:/Development/Modeling/Modeling challenge/ToyotaSupra/Supra.FBX");
    
    loaded_animations BearSuccess = LoadSkeletalAnimations("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Bear/animations/Success.FBX");
    loaded_animations BearIdle = LoadSkeletalAnimations("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Bear/animations/Idle.FBX");
    loaded_animations FoxTalk = LoadSkeletalAnimations("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Fox/animations/Talk.FBX");
    
    A->BearSuccess = BearSuccess.Animations[0];
    A->BearIdle = BearIdle.Animations[0];
    A->FoxTalk = FoxTalk.Animations[0];
    
    // NOTE(Dima): Bear materials
    A->BearMaterial = {};
    A->BearMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->BearDiffuse;
    
    A->BearEyesMaterial = {};
    A->BearEyesMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->BearEyesDiffuse;
    
    A->BearEyesShineMaterial = {};
    A->BearEyesShineMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->BearEyesShine;
    
    A->Bear.Materials[0] = &A->BearMaterial;
    A->Bear.Materials[1] = &A->BearEyesMaterial;
    A->Bear.Materials[2] = &A->BearEyesShineMaterial;
    A->Bear.Meshes[1]->MaterialIndexInModel = 1;
    A->Bear.Meshes[2]->MaterialIndexInModel = 2;
    A->Bear.Meshes[3]->MaterialIndexInModel = 1;
    A->Bear.Meshes[4]->MaterialIndexInModel = 2;
    
    // NOTE(Dima): Fox materials
    A->FoxMaterial = {};
    A->FoxMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->FoxDiffuse;
    
    A->FoxEyesMaterial = {};
    A->FoxEyesMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->FoxEyesDiffuse;
    
    A->FoxEyesShineMaterial = {};
    A->FoxEyesShineMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->FoxEyesShine;
    
    A->Fox.Materials[0] = &A->FoxMaterial;
    A->Fox.Materials[1] = &A->FoxEyesMaterial;
    A->Fox.Materials[2] = &A->FoxEyesShineMaterial;
    A->Fox.Meshes[1]->MaterialIndexInModel = 1;
    A->Fox.Meshes[2]->MaterialIndexInModel = 2;
    A->Fox.Meshes[3]->MaterialIndexInModel = 1;
    A->Fox.Meshes[4]->MaterialIndexInModel = 2;
    
    // NOTE(Dima): Other materials
    A->PaletteMaterial = {};
    A->PaletteMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->Palette;
    
    A->GroundMaterial = {};
    A->GroundMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->PlaneTexture;
    
    // NOTE(Dima): Supra material
    A->Supra.Materials[0] = &A->PaletteMaterial;
#endif
    
#if 0
    {
        asset_pack* Pack = CreateAssetPack("common");
        
        loading_params VoxelAtlasParams = DefaultLoadingParams();
        VoxelAtlasParams.Image_FilteringIsClosest = true;
        AddAssetImage(Pack, "Image_VoxelAtlas1", 
                      "../Data/Textures/minc_atlas1.png",
                      VoxelAtlasParams);
        
        AddAssetImage(Pack, "Image_VoxelAtlas2", 
                      "../Data/Textures/minc_atlas2.png",
                      VoxelAtlasParams);
        
        AddAssetImage(Pack, "Image_BoxDiffuse", 
                      "../Data/Textures/container_diffuse.png");
        
        AddAssetImage(Pack, "Image_PlaneTexture", 
                      "../Data/Textures/PixarTextures/png/fabric/Flower_pattern_pxr128.png");
        
        loading_params PaletteParams = DefaultLoadingParams();
        PaletteParams.Image_FilteringIsClosest = true;
        AddAssetImage(Pack, "Image_Palette", 
                      "../Data/Textures/MyPallette.png");
        
        loading_params BerlinSansParams = DefaultLoadingParams();
        BerlinSansParams.Font_PixelHeight = 60;
        AddAssetFont("Font_BerlinSans", 
                     "../Data/Fonts/BerlinSans.ttf",
                     BerlinSansParams);
        
        loading_params LibMonoParams = DefaultLoadingParams();
        LibMonoParams.Font_PixelHeight = 24;
        AddAssetFont("Font_LiberationMono", 
                     "../Data/Fonts/liberation-mono.ttf",
                     LibMonoParams);
        
        AddAssetInternal("Mesh_Cube", Asset_Mesh, &A->Cube);
        AddAssetInternal("Mesh_Plane", Asset_Mesh, &A->Plane);
        
        AddAssetSkybox("Skybox_Default", 
                       "../Data/Textures/Cubemaps/Pink/left.png",
                       "../Data/Textures/Cubemaps/Pink/right.png",
                       "../Data/Textures/Cubemaps/Pink/front.png",
                       "../Data/Textures/Cubemaps/Pink/back.png",
                       "../Data/Textures/Cubemaps/Pink/up.png",
                       "../Data/Textures/Cubemaps/Pink/down.png");
        
        WriteAssetPackToFile(Pack);
    }
#endif
    
    
#if 0    
    AddAsset("Cubemap_Sky", Asset_Cubemap, &A->Sky);
    
    
    AddAsset("Image_BearDiffuse", Asset_Image, &A->BearDiffuse);
    AddAsset("Image_BearNormal", Asset_Image, &A->BearNormal);
    AddAsset("Image_BearEyesDiffuse", Asset_Image, &A->BearEyesDiffuse);
    AddAsset("Image_BearEyesShine", Asset_Image, &A->BearEyesShine);
    
    AddAsset("Image_FoxDiffuse", Asset_Image, &A->FoxDiffuse);
    AddAsset("Image_FoxNormal", Asset_Image, &A->FoxNormal);
    AddAsset("Image_FoxEyesDiffuse", Asset_Image, &A->FoxEyesDiffuse);
    AddAsset("Image_FoxEyesShine", Asset_Image, &A->FoxEyesShine);
    
    AddAsset("Model_Bear", Asset_Model, &A->Bear);
    AddAsset("Model_Fox", Asset_Model, &A->Fox);
    AddAsset("Model_Supra", Asset_Model, &A->Supra);
#endif
}
