
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
                         ArrLen(StringsToConcat));
    
    return (Buf);
}


INTERNAL_FUNCTION inline 
asset* GetAssetByID(asset_storage* Storage, asset_id ID)
{
    asset* Asset = &Storage->Assets[ID];
    
    return Asset;
}

INTERNAL_FUNCTION 
asset_id AddAssetToStorage(asset_storage* Storage, 
                           char* GUID,
                           u32 Type)
{
    asset_id NewAssetID = Storage->NumAssets++;
    asset* NewAsset = GetAssetByID(Storage, NewAssetID);
    
    NewAsset->Type = Type;
    CopyStringsSafe(NewAsset->GUID, ArrLen(NewAsset->GUID), GUID);
    
    
    // NOTE(Dima): Adding asset to (Guid to AssetID) mapping
    u32 GuidHash = StringHashFNV(GUID);
    asset_hashmap_entry* FoundEntry = Storage->GuidToID.find(GuidHash);
    
    /*
    // NOTE(Dima): When inserting asset to hashmap we have to make sure 
that asset with the same GUID has not been inserted into asset storage before.
*/
    Assert(!FoundEntry);
    
    asset_hashmap_entry NewEntry = {};
    NewEntry.AssetGuidHash = GuidHash;
    NewEntry.AssetID = NewAssetID;
    
    Storage->GuidToID.insert(GuidHash, NewEntry);
    
    // TODO(Dima): Maybe allocate header here
    
    return NewAssetID;
}

INTERNAL_FUNCTION void InitAssetStorage(asset_storage* Storage)
{
    Assert(Storage->NumAssets == 0);
    Assert(Storage->Initialized == false);
    
    AddAssetToStorage(Storage, "NullAsset", Asset_None);
    
    Storage->Initialized = true;
    Storage->NumAssets = 0;
    Storage->Arena = {};
    Storage->GuidToID = hashmap<asset_hashmap_entry, ASSET_STORAGE_HASHMAP_SIZE>(&Storage->Arena);
}

INTERNAL_FUNCTION void InitAssetLoadingContext(asset_loading_context* Ctx)
{
    for (int PackIndex = 0;
         PackIndex < MAX_ASSET_PACKS;
         PackIndex++)
    {
        asset_pack* Pack = &Ctx->Packs[PackIndex];
        
        Pack->InUse = false;
        Pack->IndexInPacks = -1;
    }
}

INTERNAL_FUNCTION asset_pack* CreateAssetPack(asset_loading_context* Ctx, char* PackName)
{
    asset_pack* Pack = 0;
    
    for (int PackIndex = 0;
         PackIndex < MAX_ASSET_PACKS;
         PackIndex++)
    {
        asset_pack* CurPack = &Ctx->Packs[PackIndex];
        
        if (CurPack->InUse == false)
        {
            Pack = CurPack;
            Pack->IndexInPacks = PackIndex;
            break;
        }
    }
    
    Assert(Pack);
    
    InitAssetStorage(&Pack->AssetStorage);
    
    ClearString(Pack->PackFileName, ArrLen(Pack->PackFileName));
    AppendToString(Pack->PackBlobName, ArrLen(Pack->PackBlobName), PackName);
    AppendToString(Pack->PackBlobName, ArrLen(Pack->PackBlobName), ".pack");
    
    return Pack;
}

INTERNAL_FUNCTION void FreeAssetPack(asset_pack* Pack)
{
    Assert(Pack->InUse);
    
    Pack->InUse = false;
    Pack->IndexInPacks = -1;
    
    // TODO(Dima): Free all assets in asset pack
    
}

INTERNAL_FUNCTION void 
WriteAssetPackToFile(asset_pack* Pack)
{
    
}

#if 0
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

INTERNAL_FUNCTION asset_id AddAssetImage(asset_pack* Pack, 
                                         const char* GUID, 
                                         const char* Path,
                                         const loading_params& Params = LoadingParams_Image())
{
    image* Image = ;
}

INTERNAL_FUNCTION asset_id AddAssetSkybox(asset_pack* Pack,
                                          const char* GUID,
                                          asset_id LeftID,
                                          asset_id RightID,
                                          asset_id FrontID,
                                          asset_id BackID,
                                          asset_id RightID,
                                          asset_id UpID,
                                          asset_id DownID)
{
    
}

INTERNAL_FUNCTION asset_id AddAssetSkybox(asset_pack* Pack,
                                          const char* GUID,
                                          const char* LeftPath,
                                          const char* RightPath,
                                          const char* FrontPath,
                                          const char* BackPath,
                                          const char* UpPath,
                                          const char* DownPath,
                                          const loading_params& ImagesParams = LoadingParams_Image())
{
    char GuidBuf[128];
    
    GenerateSpecialGUID(GuidBuf, ARC(GuidBuf), GUID, "Left");
    asset_id LeftID = AddAssetImage(Pack, GuidBuf, LeftPath, ImagesParams);
    
    GenerateSpecialGUID(GuidBuf, ARC(GuidBuf), GUID, "Right");
    asset_id RightID = AddAssetImage(Pack, GuidBuf, RightPath, ImagesParams);
    
    GenerateSpecialGUID(GuidBuf, ARC(GuidBuf), GUID, "Front");
    asset_id FrontID = AddAssetImage(Pack, GuidBuf, FrontPath, ImagesParams);
    
    GenerateSpecialGUID(GuidBuf, ARC(GuidBuf), GUID, "Back");
    asset_id BackID = AddAssetImage(Pack, GuidBuf, BackPath, ImagesParams);
    
    GenerateSpecialGUID(GuidBuf, ARC(GuidBuf), GUID, "Up");
    asset_id UpID = AddAssetImage(Pack, GuidBuf, UpPath, ImagesParams);
    
    GenerateSpecialGUID(GuidBuf, ARC(GuidBuf), GUID, "Down");
    asset_id DownID = AddAssetImage(Pack, GuidBuf, DownPath, ImagesParams);
    
    asset_id Result = AddAssetSkybox(Pack, GUID,
                                     LeftID, RightID,
                                     FrontID, BackID,
                                     UpID, DownID);
    
    return Result;
}

INTERNAL_FUNCTION asset_system AddAssetModel(asset_pack* Pack,
                                             const char* GUID,
                                             const char* FilePath,
                                             const loading_params& Params)
{
    
}

#endif

INTERNAL_FUNCTION void InitAssetSystem(memory_arena* Arena)
{
    Global_Assets = PushStruct(Arena, asset_system);
    Global_Assets->Arena = Arena;
    
    asset_system* A = Global_Assets;
    
    // NOTE(Dima): Font atlas initializing
#if 1
    loading_params VoxelAtlasParams = LoadingParams_Image();
    VoxelAtlasParams.Image.FilteringIsClosest = true;
    A->VoxelAtlas = LoadImageFile("../Data/Textures/minc_atlas2.png", VoxelAtlasParams);
    //A->VoxelAtlas = LoadImageFile("../Data/Textures/minc_atlas1.jpg", VoxelAtlasParams);
    
#if 0
    // NOTE(Dima): Loading fonts
    A->TimesNewRoman = LoadFontFile("C:/Windows/Fonts/times.ttf");
    A->LifeIsGoofy = LoadFontFile("../Data/Fonts/Life is goofy.ttf");
    A->Arial = LoadFontFile("c:/windows/fonts/arial.ttf");
#endif
    
    A->BerlinSans = LoadFontFile("../Data/Fonts/BerlinSans.ttf");
    A->LiberationMono = LoadFontFile("../Data/Fonts/liberation-mono.ttf");
    A->Dimbo = LoadFontFile("../Data/Fonts/Dimbo Regular.ttf");
    
#if 0    
    AddFontToAtlas(&A->TimesNewRoman);
    AddFontToAtlas(&A->LifeIsGoofy);
    AddFontToAtlas(&A->Arial);
#endif
    
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
    loading_params PaletteParams = LoadingParams_Image();
    PaletteParams.Image.FilteringIsClosest = true;
    A->Palette = LoadImageFile("E:/Development/Modeling/Pallette/MyPallette.png", PaletteParams);
    
    A->BearDiffuse = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Bear/Bear.tga");
    A->BearNormal = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Bear/Bear Normals.tga");
    A->BearEyesDiffuse = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Bear/Eye Bear.tga");
    A->BearEyesShine = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Bear/Eye Shine Bear.tga");
    
    A->FoxDiffuse = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Fox/Fox.tga");
    A->FoxNormal = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Fox/Fox Normals.tga");
    A->FoxEyesDiffuse = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Fox/Eye Green.tga");
    A->FoxEyesShine = LoadImageFile("E:/Development/Modeling/3rdParty/ForestAnimals/Textures/Fox/Eye Shine.tga");
    
    loading_params BearParams = LoadingParams_Model();
    BearParams.Model.DefaultScale = 0.01f;
    BearParams.Model.FixInvalidRotation = true;
    
    loading_params FoxParams = BearParams;
    
    A->Bear = LoadModel("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Bear/bear.FBX", BearParams);
    A->Fox = LoadModel("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Fox/Fox.FBX", FoxParams);
    
    A->Supra = LoadModel("E:/Development/Modeling/Modeling challenge/ToyotaSupra/Supra.FBX");
    A->Mustang = LoadModel("E:/Development/Modeling/Modeling challenge/MustangGTGen6/mustanggt6gen.fbx");
    A->NissanGTR = LoadModel("E:/Development/Modeling/Modeling challenge/NissanGTR/NissanGTR.fbx");
    A->Golf2 = LoadModel("E:/Development/Modeling/Modeling challenge/Golf2/golf2.fbx");
    A->Aventador = LoadModel("E:/Development/Modeling/Modeling challenge/LambAventador/aventador.fbx");
    
    A->BearSuccess = LoadFirstSkeletalAnimation("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Bear/animations/Success.FBX");
    A->BearIdle = LoadFirstSkeletalAnimation("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Bear/animations/Idle.FBX");
    A->FoxTalk = LoadFirstSkeletalAnimation("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Fox/animations/Talk.FBX");
    
    // NOTE(Dima): Bear materials
    A->BearMaterial = {};
    A->BearMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->BearDiffuse;
    
    A->BearEyesMaterial = {};
    A->BearEyesMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->BearEyesDiffuse;
    
    A->BearEyesShineMaterial = {};
    A->BearEyesShineMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->BearEyesShine;
    
    A->Bear->Materials[0] = &A->BearMaterial;
    A->Bear->Materials[1] = &A->BearEyesMaterial;
    A->Bear->Materials[2] = &A->BearEyesShineMaterial;
#if 0
    A->Bear.Meshes[1]->MaterialIndexInModel = 1;
    A->Bear.Meshes[2]->MaterialIndexInModel = 2;
    A->Bear.Meshes[3]->MaterialIndexInModel = 1;
    A->Bear.Meshes[4]->MaterialIndexInModel = 2;
#endif
    
    // NOTE(Dima): Fox materials
    A->FoxMaterial = {};
    A->FoxMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->FoxDiffuse;
    
    A->FoxEyesMaterial = {};
    A->FoxEyesMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->FoxEyesDiffuse;
    
    A->FoxEyesShineMaterial = {};
    A->FoxEyesShineMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->FoxEyesShine;
    
    A->Fox->Materials[0] = &A->FoxMaterial;
    A->Fox->Materials[1] = &A->FoxEyesMaterial;
    A->Fox->Materials[2] = &A->FoxEyesShineMaterial;
    
#if 0    
    A->Fox.Meshes[1]->MaterialIndexInModel = 1;
    A->Fox.Meshes[2]->MaterialIndexInModel = 2;
    A->Fox.Meshes[3]->MaterialIndexInModel = 1;
    A->Fox.Meshes[4]->MaterialIndexInModel = 2;
#endif
    
    // NOTE(Dima): Other materials
    A->PaletteMaterial = {};
    A->PaletteMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->Palette;
    
    A->GroundMaterial = {};
    A->GroundMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->PlaneTexture;
    
    // NOTE(Dima): Supra material
    A->Supra->Materials[0] = &A->PaletteMaterial;
    A->Mustang->Materials[0] = &A->PaletteMaterial;
    A->NissanGTR->Materials[0] = &A->PaletteMaterial;
    A->Golf2->Materials[0] = &A->PaletteMaterial;
    A->Aventador->Materials[0] = &A->PaletteMaterial;
    
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
        
        AddAssetFont(Pack, "Font_BerlinSans", "../Data/Fonts/BerlinSans.ttf");
        AddAssetFont(Pack, "Font_LiberationMono", "../Data/Fonts/liberation-mono.ttf");
        
        AddAssetInternal(Pack, "Mesh_Cube", Asset_Mesh, &A->Cube);
        AddAssetInternal(Pack, "Mesh_Plane", Asset_Mesh, &A->Plane);
        
        AddAssetSkybox(Pack, "Skybox_Default", 
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

