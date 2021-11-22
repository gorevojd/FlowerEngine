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

INTERNAL_FUNCTION inline
void* GetAssetDataByID_(asset_storage* Storage, asset_id ID)
{
    asset* Asset = GetAssetByID(Storage, ID);
    
    return Asset->DataPtr.Ptr;
}

#define GetAssetDataByID(storage, id, type) (type*)GetAssetDataByID_(storage, id)

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

INTERNAL_FUNCTION asset_source* AddSourceToAsset(asset* Asset, mi SizeofSourceStruct)
{
    
}

INTERNAL_FUNCTION void InitAssetStorage(asset_storage* Storage)
{
    Assert(Storage->NumAssets == 0);
    Assert(Storage->Initialized == false);
    
    Storage->Initialized = true;
    Storage->NumAssets = 0;
    Storage->Arena = {};
    Storage->GuidToID = hashmap<asset_hashmap_entry, ASSET_STORAGE_HASHMAP_SIZE>(&Storage->Arena);
    
    AddAssetToStorage(Storage, "NullAsset", Asset_None);
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

INTERNAL_FUNCTION asset_pack* UseAssetPack(asset_loading_context* Ctx, char* PackName)
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
    
    asset_storage* Storage = &Pack->AssetStorage;
    
    // NOTE(Dima): Freeing all assets sources & data pointers & headers in asset pack
    for (int AssetIndex = 1;
         AssetIndex < Storage->NumAssets;
         AssetIndex++)
    {
        asset* Asset = &Storage->Assets[AssetIndex];
        
        // NOTE(Dima): Freeing DataPtr if it is set
        if (Asset->DataPtr.Ptr)
        {
            free(Asset->DataPtr.Ptr);
        }
        Asset->DataPtr.Ptr = 0;
    }
}

INTERNAL_FUNCTION void 
WriteAssetPackToFile(asset_pack* Pack)
{
    
}

INTERNAL_FUNCTION
asset_id AddAssetImage(asset_storage* Storage,
                       char* GUID,
                       image* Image)
{
    asset_id Result = AddAssetToStorage(Storage, GUID, Asset_Image);
    asset* Asset = GetAssetByID(Storage, Result);
    
    // NOTE(Dima): Filling Data
    Asset->DataPtr.Image = Image;
    
    // NOTE(Dima): Filling header
    asset_header_image* Header = &Asset->Header.Image;
    
    Header->Width = Image->Width;
    Header->Height = Image->Height;
    Header->FilteringIsNearest = Image->FilteringIsClosest;
    Header->AlignX = Image->Align.x;
    Header->AlignY = Image->Align.y;
    Header->BlobDataOffset = 0;
    
    return Result;
}

INTERNAL_FUNCTION 
asset_id AddAssetImage(asset_storage* Storage, 
                       char* GUID, 
                       char* Path,
                       loading_params& Params = LoadingParams_Image())
{
    // NOTE(Dima): Loading image
    image* Image = LoadImageFile(Path, Params);
    
    asset_id Result = AddAssetImage(Storage, GUID, Image);
    asset* Asset = GetAssetByID(Storage, Result);
    
    // NOTE(Dima): Filling asset source
    asset_source* Source = &Asset->Source;
    CopyStringsSafe(Source->FilePath,
                    ArrLen(Source->FilePath),
                    Path);
    Source->Params = Params;
    
    return Result;
}

INTERNAL_FUNCTION 
asset_id AddAssetCubemap(asset_storage* Storage,
                         char* GUID,
                         asset_id LeftID,
                         asset_id RightID,
                         asset_id FrontID,
                         asset_id BackID,
                         asset_id UpID,
                         asset_id DownID)
{
    asset_id Result = AddAssetToStorage(Storage, GUID, Asset_Cubemap);
    
    asset* Asset = GetAssetByID(Storage, Result);
    
    cubemap* Cubemap = (cubemap*)malloc(sizeof(cubemap));
    
    // NOTE(Dima): Setting up DataPtr
    asset* AssetImgLeft = GetAssetByID(Storage, LeftID);
    asset* AssetImgRight = GetAssetByID(Storage, RightID);
    asset* AssetImgFront = GetAssetByID(Storage, FrontID);
    asset* AssetImgBack = GetAssetByID(Storage, BackID);
    asset* AssetImgUp = GetAssetByID(Storage, UpID);
    asset* AssetImgDown = GetAssetByID(Storage, DownID);
    
    Cubemap->Left = AssetImgLeft->DataPtr.Image;
    Cubemap->Right = AssetImgRight->DataPtr.Image;
    Cubemap->Front = AssetImgFront->DataPtr.Image;
    Cubemap->Back = AssetImgBack->DataPtr.Image;
    Cubemap->Top = AssetImgUp->DataPtr.Image;
    Cubemap->Down = AssetImgDown->DataPtr.Image;
    
    Asset->DataPtr.Cubemap = Cubemap;
    
    // NOTE(Dima): Setting up asset header
    asset_header_cubemap* Header = &Asset->Header.Cubemap;
    
    Header->Left = LeftID;
    Header->Right = RightID;
    Header->Front = FrontID;
    Header->Back = BackID;
    Header->Up = UpID;
    Header->Down = DownID;
    
    return Result;
}

INTERNAL_FUNCTION 
asset_id AddAssetCubemap(asset_storage* Storage,
                         char* GUID,
                         char* LeftPath,
                         char* RightPath,
                         char* FrontPath,
                         char* BackPath,
                         char* UpPath,
                         char* DownPath,
                         loading_params& ImagesParams = LoadingParams_Image())
{
    char GuidBuf[128];
    
    GenerateSpecialGUID(GuidBuf, ArrLen(GuidBuf), GUID, "Left");
    asset_id LeftID = AddAssetImage(Storage, GuidBuf, LeftPath, ImagesParams);
    
    GenerateSpecialGUID(GuidBuf, ArrLen(GuidBuf), GUID, "Right");
    asset_id RightID = AddAssetImage(Storage, GuidBuf, RightPath, ImagesParams);
    
    GenerateSpecialGUID(GuidBuf, ArrLen(GuidBuf), GUID, "Front");
    asset_id FrontID = AddAssetImage(Storage, GuidBuf, FrontPath, ImagesParams);
    
    GenerateSpecialGUID(GuidBuf, ArrLen(GuidBuf), GUID, "Back");
    asset_id BackID = AddAssetImage(Storage, GuidBuf, BackPath, ImagesParams);
    
    GenerateSpecialGUID(GuidBuf, ArrLen(GuidBuf), GUID, "Up");
    asset_id UpID = AddAssetImage(Storage, GuidBuf, UpPath, ImagesParams);
    
    GenerateSpecialGUID(GuidBuf, ArrLen(GuidBuf), GUID, "Down");
    asset_id DownID = AddAssetImage(Storage, GuidBuf, DownPath, ImagesParams);
    
    asset_id Result = AddAssetCubemap(Storage, 
                                      GUID,
                                      LeftID, 
                                      RightID,
                                      FrontID, 
                                      BackID,
                                      UpID, 
                                      DownID);
    
    return Result;
}

INTERNAL_FUNCTION 
asset_id AddAssetMesh(asset_storage* Storage,
                      char* GUID,
                      mesh* Mesh)
{
    asset_id Result = AddAssetToStorage(Storage, GUID, Asset_Mesh);
    asset* Asset = GetAssetByID(Storage, Result);
    
    Asset->DataPtr.Mesh = Mesh;
    
    // NOTE(Dima): Filling asset header
    asset_header_mesh* Header = &Asset->Header.Mesh;
    
    Header->VertexCount = Mesh->VertexCount;
    Header->IndexCount = Mesh->IndexCount;
    Header->IsSkinned = Mesh->IsSkinned;
    Header->MaterialIndexInModel = Mesh->MaterialIndexInModel;
    
    return Result;
}

INTERNAL_FUNCTION
asset_id AddAssetMaterial(asset_storage* Storage,
                          char* GUID,
                          char* MaterialName,
                          u32 MaterialType,
                          material* Material = 0)
{
    asset_id Result = AddAssetToStorage(Storage, GUID, Asset_Material);
    asset* Asset = GetAssetByID(Storage, Result);
    
    // NOTE(Dima): Filling DataPtr
    if (Material == 0)
    {
        Material = (material*)malloc(sizeof(material));
    }
    
    Asset->DataPtr.Material = Material;
    
    // NOTE(Dima): Filling header
    asset_header_material* Header = &Asset->Header.Material;
    
    CopyStrings(Header->Name, MaterialName);
    Header->Type = MaterialType;
    
    return Result;
}

INTERNAL_FUNCTION
void SetMaterialTexture(asset_storage* Storage,
                        asset_id MaterialID,
                        u32 TextureType,
                        asset_id TextureID)
{
    // NOTE(Dima): Checking if asset types are correct
    asset* MaterialAsset = GetAssetByID(Storage, MaterialID);
    Assert(Asset_Material == MaterialAsset->Type);
    
    asset* ImageAsset = GetAssetByID(Storage, TextureID);
    Assert(Asset_Image == ImageAsset->Type);
    
    // NOTE(Dima): Setting up image in header
    asset_header_material* Header = &MaterialAsset->Header.Material;
    Header->TextureIDs[TextureType] = TextureID;
    
    // NOTE(Dima): Setting up image in actual 'material' pointer
    material* Material = GetAssetDataByID(Storage, MaterialID, material);
    image* Image = GetAssetDataByID(Storage, TextureID, image);
    Material->Textures[TextureType] = Image;
}

INTERNAL_FUNCTION 
asset_id AddAssetMaterial(asset_storage* Storage,
                          char* GUID,
                          material* Material)
{
    asset_id Result = AddAssetMaterial(Storage, 
                                       GUID, 
                                       Material->Name,
                                       Material->Type,
                                       Material);
    asset* Asset = GetAssetByID(Storage, Result);
    
    // NOTE(Dima): Filling DataPtr
    Asset->DataPtr.Material = Material;
    
    // NOTE(Dima): Filling asset header. 
    // NOTE(Dima): (Material Name & Type are already set by AddAssetMaterialInternal)
    asset_header_material* Header = &Asset->Header.Material;
    
    for (int TextureIndex = 0;
         TextureIndex < MAX_MATERIAL_TEXTURES;
         TextureIndex++)
    {
        image* Image = Material->Textures[TextureIndex];
        if (Image)
        {
            char TempBuf[16];
            IntegerToString(TextureIndex, TempBuf);
            
            char ImageGUID[ASSET_GUID_SIZE];
            GenerateSpecialGUID(ImageGUID,
                                ASSET_GUID_SIZE,
                                GUID,
                                TempBuf);
            
            asset_id ImageID = AddAssetImage(Storage, 
                                             ImageGUID,
                                             Image);
            
            Header->TextureIDs[TextureIndex] = ImageID;
        }
        else
        {
            Header->TextureIDs[TextureIndex] = 0;
        }
    }
    
    return Result;
}

INTERNAL_FUNCTION 
void SetModelMaterial(asset_storage* Storage, 
                      asset_id ModelID,
                      int MaterialIndex,
                      asset_id MaterialID)
{
    // NOTE(Dima): Checking if assets have correct types
    asset* ModelAsset = GetAssetByID(Storage, ModelID);
    Assert(Asset_Model == ModelAsset->Type);
    
    asset* MaterialAsset = GetAssetByID(Storage, MaterialID);
    Assert(Asset_Material == MaterialAsset->Type);
    
    // NOTE(Dima): Setting up model
    model* Model = GetAssetDataByID(Storage, ModelID, model);
    material* Material = GetAssetDataByID(Storage, MaterialID, material);
    
    Model->Materials[MaterialIndex] = Material;
    Model->MaterialIDs[MaterialIndex] = MaterialID;
}

INTERNAL_FUNCTION 
asset_id AddAssetModel(asset_storage* Storage,
                       char* GUID,
                       char* FilePath,
                       const loading_params& Params = LoadingParams_Model())
{
    asset_id Result = AddAssetToStorage(Storage, GUID, Asset_Model);
    asset* Asset = GetAssetByID(Storage, Result);
    
    model* Model = LoadModel(FilePath, Params);
    
    Asset->DataPtr.Model = Model;
    
    // NOTE(Dima): Filling asset source
    asset_source* Source = &Asset->Source;
    Source->Params = Params;
    CopyStringsSafe(Source->FilePath, 
                    ArrLen(Source->FilePath),
                    FilePath);
    
    // NOTE(Dima): Filling asset header
    asset_header_model* Header = &Asset->Header.Model;
    Header->NumMeshes = Model->Meshes.size();
    Header->NumMaterials = Model->Materials.size();
    Header->NumNodes = Model->NumNodes;
    Header->NumBones = Model->NumBones;
    Header->NumNodesMeshIndices = Model->NumNodesMeshIndices;
    Header->NumNodesChildIndices = Model->NumNodesChildIndices;
    
    Model->MeshIDs.resize(Model->Meshes.size(), 0);
    Model->MaterialIDs.resize(Model->Materials.size(), 0);
    
    // NOTE(Dima): Fillling material IDs
    for (int MaterialIndex = 0;
         MaterialIndex < Model->Materials.size();
         MaterialIndex++)
    {
        material* Material = Model->Materials.at(MaterialIndex);
        
        asset_id MaterialAssetID = 0;
        if (Material)
        {
            char TempBuf[32];
            stbsp_sprintf(TempBuf, "_Material:%d", MaterialIndex);
            
            char MaterialGUID[ASSET_GUID_SIZE];
            GenerateSpecialGUID(MaterialGUID,
                                ASSET_GUID_SIZE,
                                GUID,
                                TempBuf);
            
            MaterialAssetID = AddAssetMaterial(Storage, GUID, Material);
        }
        
        Model->MaterialIDs[MaterialIndex] = MaterialAssetID;
    }
    
    // NOTE(Dima): Filling mesh IDs
    for (int MeshIndex = 0;
         MeshIndex < Model->Meshes.size();
         MeshIndex++)
    {
        mesh* Mesh = Model->Meshes[MeshIndex];
        
        asset_id MeshAssetID = 0;
        
        if (Mesh)
        {
            char TempBuf[32];
            stbsp_sprintf(TempBuf, "_Mesh:%d", MeshIndex);
            
            char MeshGUID[ASSET_GUID_SIZE];
            GenerateSpecialGUID(MeshGUID,
                                ASSET_GUID_SIZE,
                                GUID,
                                TempBuf);
            
            MeshAssetID = AddAssetMesh(Storage, GUID, Mesh);
            
        }
        
        Model->MeshIDs[MeshIndex] = MeshAssetID;
    }
    
    return Result;
}



#if 0
INTERNAL_FUNCTION 
void AddBear(asset_storage* Storage)
{
    loading_params Params = LoadingParams_Model();
    Params.Model.DefaultScale = 0.01f;
    Params.Model.FixInvalidRotation = true;
    
    A->Bear = LoadModel("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Bear/bear.FBX", Params);
    
    // NOTE(Dima): Loading animations
    A->BearSuccess = LoadFirstSkeletalAnimation("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Bear/animations/Success.FBX");
    A->BearIdle = LoadFirstSkeletalAnimation("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Bear/animations/Idle.FBX");
    
    
    // NOTE(Dima): Setting materials
    A->BearMaterial = {};
    A->BearMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->BearDiffuse;
    
    A->BearEyesMaterial = {};
    A->BearEyesMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->BearEyesDiffuse;
    
    A->BearEyesShineMaterial = {};
    A->BearEyesShineMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->BearEyesShine;
    
    A->Bear->Materials[0] = &A->BearMaterial;
    A->Bear->Materials[1] = &A->BearEyesMaterial;
    A->Bear->Materials[2] = &A->BearEyesShineMaterial;
}

INTERNAL_FUNCTION 
void AddFox(asset_storage* Storage)
{
    loading_params Params = LoadingParams_Model();
    Params.Model.DefaultScale = 0.01f;
    Params.Model.FixInvalidRotation = true;
    
    A->Fox = LoadModel("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Fox/Fox.FBX", Params);
    
    A->FoxTalk = LoadFirstSkeletalAnimation("E:/Development/Modeling/3rdParty/ForestAnimals/FBX/Fox/animations/Talk.FBX");
    
    
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
    
}
#endif

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

INTERNAL_FUNCTION void InitAssetSystem(memory_arena* Arena)
{
    Global_Assets = PushStruct(Arena, asset_system);
    Global_Assets->Arena = Arena;
    
    asset_system* A = Global_Assets;
    
#if 1
    loading_params VoxelAtlasParams = LoadingParams_Image();
    VoxelAtlasParams.Image.FilteringIsClosest = true;
    A->VoxelAtlas = LoadImageFile("../Data/Textures/minc_atlas2.png", VoxelAtlasParams);
    //A->VoxelAtlas = LoadImageFile("../Data/Textures/minc_atlas1.jpg", VoxelAtlasParams);
    
    A->BerlinSans = LoadFontFile("../Data/Fonts/BerlinSans.ttf");
    A->LiberationMono = LoadFontFile("../Data/Fonts/liberation-mono.ttf");
    A->Dimbo = LoadFontFile("../Data/Fonts/Dimbo Regular.ttf");
    
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
    A->PaletteMaterial.Textures[0] = A->Palette;
    
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
    
    // NOTE(Dima): Other materials
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
}

