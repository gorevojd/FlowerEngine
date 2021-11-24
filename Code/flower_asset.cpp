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
asset_id GetAssetID(asset_storage* Storage, char* GUID)
{
    asset_id Result = 0;
    
    auto& Found = Storage->GuidToID.find(GUID);
    if (Found != Storage->GuidToID.end())
    {
        Result = Found->second;
    }
    
    return Result;
}

INTERNAL_FUNCTION inline 
asset_id GetAssetID(char* GUID)
{
    asset_id Result = GetAssetID(&Global_Assets->AssetStorage, GUID);
    
    return Result;
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
#define G_GetAssetDataByID(id, type) (type*)GetAssetDataByID_(&Global_Assets->AssetStorage, id)


INTERNAL_FUNCTION
void ProcessAssetHeader(asset* Asset, bool IsInit)
{
    asset_header* Header = &Asset->Header;
    
    Header->Ptr = Header->HeaderBytes;
    
    switch(Asset->Type)
    {
        case Asset_Model:
        {
            if (IsInit)
            {
                new (Asset->Header.Model) asset_header_model;
            }
            else
            {
                Asset->Header.Model->~asset_header_model();
            }
        }break;
    }
    
    if (!IsInit)
    {
        Header->Ptr = 0;
    }
    else if (IsInit && (Asset->Type != Asset_None))
    {
        StrictAssert(Asset->Header.Ptr);
    }
}

INTERNAL_FUNCTION 
asset_id AddAssetToStorage(asset_storage* Storage, 
                           char* GUID,
                           u32 Type)
{
    asset_id NewAssetID = Storage->NumAssets++;
    asset* NewAsset = GetAssetByID(Storage, NewAssetID);
    
    NewAsset->IsSupplemental = false;
    NewAsset->Type = Type;
    CopyStringsSafe(NewAsset->GUID, ArrLen(NewAsset->GUID), GUID);
    
    
    // NOTE(Dima): Adding asset to (Guid to AssetID) mapping
    b32 FoundWithTheSameName = false;
    if (Storage->GuidToID.size() > 0)
    {
        auto Found = Storage->GuidToID.find(NewAsset->GUID);
        
        FoundWithTheSameName = Found != Storage->GuidToID.end();
    }
    
    /*
    // NOTE(Dima): When inserting asset to hashmap we have to make sure 
that asset with the same GUID has not been inserted into asset storage before.
*/
    Assert(!FoundWithTheSameName);
    
    Storage->GuidToID[NewAsset->GUID] = NewAssetID;
    
    // NOTE(Dima): Initializing header
    ProcessAssetHeader(NewAsset, true);
    
    return NewAssetID;
}

INTERNAL_FUNCTION
void FreeAsset(asset_storage* Storage, int AssetIndex)
{
    asset* Asset = &Storage->Assets[AssetIndex];
    
    // NOTE(Dima): Freeing DataPtr if it is set
    if (!Asset->IsSupplemental && Asset->DataPtr.Ptr)
    {
        free(Asset->DataPtr.Ptr);
    }
    Asset->DataPtr.Ptr = 0;
    
    // NOTE(Dima): Freeing asset header
    ProcessAssetHeader(Asset, false);
}

INTERNAL_FUNCTION void InitAssetStorage(asset_storage* Storage)
{
    Assert(Storage->NumAssets == 0);
    Assert(Storage->Initialized == false);
    
    new (Storage) asset_storage;
    
    Storage->Initialized = true;
    Storage->NumAssets = 0;
    Storage->GuidToID.reserve(ASSET_DEFAULT_COUNT_IN_TABLE);
    
    AddAssetToStorage(Storage, "NullAsset", Asset_None);
}

INTERNAL_FUNCTION void FreeAssetStorage(asset_storage* Storage)
{
    // NOTE(Dima): Freeing all assets sources & data pointers & headers in asset pack
    for (int AssetIndex = 1;
         AssetIndex < Storage->NumAssets;
         AssetIndex++)
    {
        // NOTE(Dima): Freeing asset header
        FreeAsset(Storage, AssetIndex);
    }
    
    Storage->~asset_storage();
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
    
    FreeAssetStorage(Storage);
}

INTERNAL_FUNCTION void 
WriteAssetPackToFile(asset_pack* Pack)
{
    
}

INTERNAL_FUNCTION
asset_id AddAssetImage(asset_storage* Storage,
                       char* GUID,
                       image* Image,
                       b32 IsSupplemental = false)
{
    asset_id Result = AddAssetToStorage(Storage, GUID, Asset_Image);
    asset* Asset = GetAssetByID(Storage, Result);
    Asset->IsSupplemental = IsSupplemental;
    
    // NOTE(Dima): Filling Data
    Asset->DataPtr.Image = Image;
    
    // NOTE(Dima): Filling header
    asset_header_image* Header = Asset->Header.Image;
    
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
    asset_header_cubemap* Header = Asset->Header.Cubemap;
    
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
    asset_header_mesh* Header = Asset->Header.Mesh;
    
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
        
        CopyStrings(Material->Name, MaterialName);
        Material->Type = MaterialType;
    }
    
    Asset->DataPtr.Material = Material;
    
    // NOTE(Dima): Filling header
    asset_header_material* Header = Asset->Header.Material;
    
    CopyStrings(Header->Name, MaterialName);
    Header->Type = MaterialType;
    
    return Result;
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
    asset_header_material* Header = Asset->Header.Material;
    
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
    asset_header_material* Header = MaterialAsset->Header.Material;
    Header->TextureIDs[TextureType] = TextureID;
    
    // NOTE(Dima): Setting up image in actual 'material' pointer
    material* Material = GetAssetDataByID(Storage, MaterialID, material);
    image* Image = GetAssetDataByID(Storage, TextureID, image);
    Material->Textures[TextureType] = Image;
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
    ModelAsset->Header.Model->MaterialIDs[MaterialIndex] = MaterialID;
}

INTERNAL_FUNCTION 
asset_id AddAssetModel(asset_storage* Storage,
                       char* GUID,
                       char* FilePath,
                       const loading_params& Params = LoadingParams_Model())
{
    asset_id Result = AddAssetToStorage(Storage, GUID, Asset_Model);
    asset* Asset = GetAssetByID(Storage, Result);
    
    // NOTE(Dima): Loading model
    model* Model = LoadModel(FilePath, Params);
    
    // NOTE(Dima): Setting data ptr
    Asset->DataPtr.Model = Model;
    
    // NOTE(Dima): Filling asset source
    asset_source* Source = &Asset->Source;
    Source->Params = Params;
    CopyStringsSafe(Source->FilePath, 
                    ArrLen(Source->FilePath),
                    FilePath);
    
    // NOTE(Dima): Filling asset header
    asset_header_model* Header = Asset->Header.Model;
    new (Header) asset_header_model;
    
    Header->NumMeshes = Model->Meshes.size();
    Header->NumMaterials = Model->Materials.size();
    Header->NumNodes = Model->NumNodes;
    Header->NumBones = Model->NumBones;
    Header->NumNodesMeshIndices = Model->NumNodesMeshIndices;
    Header->NumNodesChildIndices = Model->NumNodesChildIndices;
    
    Header->MeshIDs.resize(Model->Meshes.size(), 0);
    Header->MaterialIDs.resize(Model->Materials.size(), 0);
    
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
            
            MaterialAssetID = AddAssetMaterial(Storage, MaterialGUID, Material);
        }
        
        Header->MaterialIDs[MaterialIndex] = MaterialAssetID;
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
            
            MeshAssetID = AddAssetMesh(Storage, MeshGUID, Mesh);
            
        }
        
        Header->MeshIDs[MeshIndex] = MeshAssetID;
    }
    
    return Result;
}

INTERNAL_FUNCTION
asset_id AddAssetNodeAnim(asset_storage* Storage,
                          char* AnimationGUID,
                          int NodeAnimIndex,
                          animation* Animation)
{
    // NOTE(Dima): Generating asset GUID
    char NodeAnimGUID[ASSET_GUID_SIZE];
    char TempBuf[ASSET_GUID_SIZE];
    
    stbsp_sprintf(TempBuf, "_NodeAnim:%d", NodeAnimIndex);
    
    GenerateSpecialGUID(NodeAnimGUID,
                        ASSET_GUID_SIZE,
                        AnimationGUID,
                        TempBuf);
    
    // NOTE(Dima): Adding asset
    asset_id Result = AddAssetToStorage(Storage, NodeAnimGUID, Asset_NodeAnimation);
    asset* Asset = GetAssetByID(Storage, Result);
    Asset->IsSupplemental = true;
    
    // NOTE(Dima): Setting data
    node_animation* NodeAnim = &Animation->NodeAnims[NodeAnimIndex];
    Asset->DataPtr.Ptr = NodeAnim;
    
    // NOTE(Dima): Filling header
    asset_header_node_animation* Header = Asset->Header.NodeAnimation;
    
    Header->NodeIndex = NodeAnim->NodeIndex;
    Header->NumPos = NodeAnim->NumPos;
    Header->NumRot = NodeAnim->NumRot;
    Header->NumSca = NodeAnim->NumScl;
    
    return Result;
}

INTERNAL_FUNCTION 
asset_id AddAssetAnimationFirst(asset_storage* Storage, 
                                char* GUID,
                                char* Path)
{
    asset_id Result = AddAssetToStorage(Storage, GUID, Asset_Animation);
    asset* Asset = GetAssetByID(Storage, Result);
    
    animation* Anim = LoadFirstSkeletalAnimation(Path);
    Assert(Anim);
    
    // NOTE(Dima): Setting data
    Asset->DataPtr.Ptr = Anim;
    
    // NOTE(Dima): Setting asset source
    CopyStringsSafe(Asset->Source.FilePath, 
                    ArrLen(Asset->Source.FilePath),
                    Path);
    
    // NOTE(Dima): Setting asset header
    asset_header_animation* Header = Asset->Header.Animation;
    
    CopyStringsSafe(Header->Name, 
                    ArrLen(Header->Name),
                    Anim->Name);
    
    Header->NumNodeAnims = Anim->NumNodeAnims;
    Header->DurationTicks = Anim->DurationTicks;
    Header->TicksPerSecond = Anim->TicksPerSecond;
    Header->OutsideBehaviour = Anim->Behaviour;
    
    // NOTE(Dima): 
    b32 IsFirstNodeAnim = true;
    for (int NodeAnimIndex = 0;
         NodeAnimIndex < Anim->NumNodeAnims;
         NodeAnimIndex++)
    {
        asset_id NodeAnimAssetID = AddAssetNodeAnim(Storage, 
                                                    GUID, 
                                                    NodeAnimIndex,
                                                    Anim);
        
        if (IsFirstNodeAnim)
        {
            Header->FirstNodeAnimID = NodeAnimAssetID;
            
            IsFirstNodeAnim = false;
        }
    }
    
    return Result;
}

INTERNAL_FUNCTION
asset_id AddAssetFontSize(asset_storage* Storage,
                          char* FontGUID,
                          int FontSizeIndex,
                          font_size* FontSize)
{
    // NOTE(Dima): Generating GUID for font size
    char FontSizeGUID[ASSET_GUID_SIZE];
    char TempBuf[ASSET_GUID_SIZE];
    stbsp_sprintf(TempBuf, 
                  "FontSize:%d",
                  FontSizeIndex);
    
    GenerateSpecialGUID(FontSizeGUID, 
                        ASSET_GUID_SIZE,
                        FontGUID,
                        TempBuf);
    
    // NOTE(Dima): Adding asset
    asset_id Result = AddAssetToStorage(Storage, FontSizeGUID, Asset_FontSize);
    asset* Asset = GetAssetByID(Storage, Result);
    Asset->IsSupplemental = true;
    
    // NOTE(Dima): Setting data ptr 
    Asset->DataPtr.Ptr = FontSize;
    
    // NOTE(Dima): Filling header
    asset_header_font_size* Header = Asset->Header.FontSize;
    
    Header->FontSizeEnumType = FontSize->FontSizeEnumType;
    Header->PixelsPerMeter = FontSize->PixelsPerMeter;
    Header->ScaleForPixelHeight = FontSize->ScaleForPixelHeight;
    
    return Result;
}

INTERNAL_FUNCTION
asset_id AddAssetGlyph(asset_storage* Storage,
                       char* FontSizeGUID,
                       int GlyphIndex,
                       glyph* Glyph)
{
    // NOTE(Dima): Generating GUID for glyph
    char GlyphGUID[ASSET_GUID_SIZE];
    char TempBuf[ASSET_GUID_SIZE];
    stbsp_sprintf(TempBuf, 
                  "Glyph:%d",
                  GlyphIndex);
    
    GenerateSpecialGUID(GlyphGUID, 
                        ASSET_GUID_SIZE,
                        FontSizeGUID,
                        TempBuf);
    
    // NOTE(Dima): Adding asset
    asset_id Result = AddAssetToStorage(Storage, FontSizeGUID, Asset_Glyph);
    asset* Asset = GetAssetByID(Storage, Result);
    Asset->IsSupplemental = true;
    
}

#if 0
INTERNAL_FUNCTION
asset_id AddAssetFont(asset_storage* Storage,
                      char* GUID,
                      char* Path,
                      const loading_params& Params = LoadingParams_Font())
{
    asset_id Result = AddAssetToStorage(Storage, GUID, Asset_Font);
    asset* Asset = GetAssetByID(Storage, Result);
    
    // NOTE(Dima): Setting asset source 
    CopyStringsSafe(Asset->Source.FilePath,
                    ArrLen(Asset->Source.FilePath),
                    Path);
    Asset->Source.Params = Params;
    
    // NOTE(Dima): Loading asset
    font* Font = LoadFontFile(Path, Params);
    
    // NOTE(Dima): Setting data ptr
    Asset->DataPtr.Ptr = Font;
    
    // NOTE(Dima): Filling header
    asset_header_font* Header = Asset->Header.Font;
    
    Header->Ascent = Font->Ascent;
    Header->Descent = Font->Descent;
    Header->LineGap = Font->LineGap;
    Header->LineAdvance = Font->LineAdvance;
    Header->UniqueNameHash = Font->UniqueNameHash;
    CopyStringsSafe(Header->UniqueName, 
                    ArrLen(Header->UniqueName),
                    Font->UniqueName);
    
    Header->NumSizes = Font->NumSizes;
    Header->NumGlyphs = Font->NumGlyphs;
    
    // NOTE(Dima): Adding atlas
    char AtlasGUID[ASSET_GUID_SIZE];
    GenerateSpecialGUID(AtlasGUID, 
                        ASSET_GUID_SIZE,
                        GUID,
                        "FontAtlas");
    
    Header->AtlasImageID = AddAssetImage(Storage,
                                         AtlasGUID,
                                         Font->Atlas,
                                         true);
    
    // NOTE(Dima): Adding font sizes
    for (int FontSizeIndex = 0;
         FontSizeIndex < Font->NumSizes;
         FontSizeIndex++)
    {
        font_size* Size = &Font->Sizes[FontSizeIndex];
        
        asset_id FontSizeAssetID = AddAssetFontSize(Storage,
                                                    GUID, 
                                                    FontSizeIndex,
                                                    Size);
        
        if (FontSizeIndex == 0)
        {
            Header->FirstFontSizeID = FontSizeAssetID;
        }
    }
    
    
    // NOTE(Dima): Adding font size's glyphs
    for (int FontSizeIndex = 0;
         FontSizeIndex < Font->NumSizes;
         FontSizeIndex++)
    {
        asset_id FontSizeID = Header->FirstFontSizeID + FontSizeIndex;
        
        asset* FontSizeAsset = GetAssetByID(Storage, FontSizeID);
        font_size* FontSize = &Font->Sizes[FontSizeIndex];
        
        asset_header_font_size* FontSizeHeader = FontSizeAsset->Header.FontSize;
        
        // NOTE(Dima): Adding font size's glyphs
        for (int GlyphIndex = 0;
             GlyphIndex < Font->NumGlyphs;
             GlyphIndex++)
        {
            asset_id GlyphID = AddAssetGlyph(???);
            
            if (GlyphIndex == 0)
            {
                FontSizeHeader->FirstGlyphId = GlyphID;
            }
        }
    }
    
    
    return Result;
}
#endif

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
#if 0    
    AddAssetFont(Pack, 
                 "Font_BerlinSans", 
                 "../Data/Fonts/BerlinSans.ttf");
    
    AddAssetFont(Pack, 
                 "Font_LiberationMono", 
                 "../Data/Fonts/liberation-mono.ttf");
    AddAssetFont(Pack,
                 "Font_Dimbo",
                 "../Data/Fonts/Dimbo Regular.ttf");
#endif
}


INTERNAL_FUNCTION 
void AddCommonAssets(asset_storage* Storage)
{
    AddCars(Storage);
    AddFonts(Storage);
    AddAnimals(Storage);
    
    
#if 0    
    AddAssetImage(Pack, "Image_BoxDiffuse", 
                  "../Data/Textures/container_diffuse.png");
    
    AddAssetImage(Pack, "Image_PlaneTexture", 
                  "../Data/Textures/PixarTextures/png/fabric/Flower_pattern_pxr128.png");
    
    AddAssetInternal(Pack, "Mesh_Cube", Asset_Mesh, &A->Cube);
    AddAssetInternal(Pack, "Mesh_Plane", Asset_Mesh, &A->Plane);
    
    AddAssetSkybox(Pack, "Skybox_Default", 
                   "../Data/Textures/Cubemaps/Pink/left.png",
                   "../Data/Textures/Cubemaps/Pink/right.png",
                   "../Data/Textures/Cubemaps/Pink/front.png",
                   "../Data/Textures/Cubemaps/Pink/back.png",
                   "../Data/Textures/Cubemaps/Pink/up.png",
                   "../Data/Textures/Cubemaps/Pink/down.png");
#endif
    
}

INTERNAL_FUNCTION void InitAssetSystem(memory_arena* Arena)
{
    Global_Assets = PushStruct(Arena, asset_system);
    Global_Assets->Arena = Arena;
    
    asset_system* A = Global_Assets;
    
    InitAssetStorage(&A->AssetStorage);
    
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
    
    // NOTE(Dima): Other materials
    A->GroundMaterial = {};
    A->GroundMaterial.Textures[MatTex_SpecularDiffuse_Diffuse] = A->PlaneTexture;
#endif
    
    {
        //asset_pack* Pack = UseAssetPack(&Global_Assets->LoadingCtx, "common");
        //asset_storage* Storage = &Pack->AssetStorage;
        
        AddCommonAssets(&Global_Assets->AssetStorage);
        
#if 0
        
        WriteAssetPackToFile(Pack);
#endif
        
#if 0
        loading_params VoxelAtlasParams = DefaultLoadingParams();
        VoxelAtlasParams.Image_FilteringIsClosest = true;
        AddAssetImage(Pack, "Image_VoxelAtlas1", 
                      "../Data/Textures/minc_atlas1.png",
                      VoxelAtlasParams);
        
        AddAssetImage(Pack, "Image_VoxelAtlas2", 
                      "../Data/Textures/minc_atlas2.png",
                      VoxelAtlasParams);
        
#endif
    }
}

