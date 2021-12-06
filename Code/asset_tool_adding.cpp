INTERNAL_FUNCTION
asset_id AddAssetImage(asset_storage* Storage,
                       const char* GUID,
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
                       const char* GUID, 
                       const char* Path,
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
                         const char* GUID,
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
                         const char* GUID,
                         const char* LeftPath,
                         const char* RightPath,
                         const char* FrontPath,
                         const char* BackPath,
                         const char* UpPath,
                         const char* DownPath,
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
                      const char* GUID,
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
                          const char* GUID,
                          const char* MaterialName,
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
                          const char* GUID,
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
                       const char* GUID,
                       const char* FilePath,
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
                          const char* AnimationGUID,
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
                                const char* GUID,
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
    
    // NOTE(Dima): Adding node animations assets
    for (int NodeAnimIndex = 0;
         NodeAnimIndex < Anim->NumNodeAnims;
         NodeAnimIndex++)
    {
        asset_id NodeAnimAssetID = AddAssetNodeAnim(Storage, 
                                                    GUID, 
                                                    NodeAnimIndex,
                                                    Anim);
        
        if (NodeAnimIndex == 0)
        {
            Header->FirstNodeAnimID = NodeAnimAssetID;
        }
    }
    
    return Result;
}

INTERNAL_FUNCTION
asset_id AddAssetFontSize(asset_storage* Storage,
                          const char* FontGUID,
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
                       const char* FontSizeGUID,
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
    asset_id Result = AddAssetToStorage(Storage, GlyphGUID, Asset_Glyph);
    asset* Asset = GetAssetByID(Storage, Result);
    Asset->IsSupplemental = true;
    
    // NOTE(Dima): Setting data ptr
    Asset->DataPtr.Glyph = Glyph;
    
    // NOTE(Dima): Filling asset header
    asset_header_glyph* Header = Asset->Header.Glyph;
    
    Header->NumStyles = Glyph->NumStyles;
    Header->Codepoint = Glyph->Codepoint;
    Header->Advance = Glyph->Advance;
    Header->LeftBearing = Glyph->LeftBearing;
    Header->XOffset = Glyph->XOffset;
    Header->YOffset = Glyph->YOffset;
    
    // NOTE(Dima): This will be setup in AddAssetFont function
    Header->FirstStyleID = 0;
    
    return Result;
}

INTERNAL_FUNCTION
asset_id AddAssetGlyphStyle(asset_storage* Storage,
                            const char* GlyphGUID,
                            int StyleIndex,
                            glyph_style* Style)
{
    char GlyphStyleGUID[ASSET_GUID_SIZE];
    char TempBuf[ASSET_GUID_SIZE];
    stbsp_sprintf(TempBuf, "GlyphStyle:%d", StyleIndex);
    
    GenerateSpecialGUID(GlyphStyleGUID,
                        ASSET_GUID_SIZE,
                        GlyphGUID,
                        TempBuf);
    
    // NOTE(Dima): Adding asset
    asset_id Result = AddAssetToStorage(Storage, GlyphStyleGUID, Asset_GlyphStyle);
    asset* Asset = GetAssetByID(Storage, Result);
    Asset->IsSupplemental = true;
    
    // NOTE(Dima): Setting data ptr
    Asset->DataPtr.GlyphStyle = Style;
    
    // NOTE(Dima): Filling asset header
    asset_header_glyph_style* Header = Asset->Header.GlyphStyle;
    
    Header->GlyphStyleType = Style->GlyphStyleType;
    Header->WidthOverHeight = Style->WidthOverHeight;
    Header->ImageHeight = Style->ImageHeight;
    Header->MinUV_x = Style->MinUV.x;
    Header->MinUV_y = Style->MinUV.y;
    Header->MaxUV_x = Style->MaxUV.x;
    Header->MaxUV_y = Style->MaxUV.y;
    
    return Result;
}

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
            glyph* Glyph = &FontSize->Glyphs[GlyphIndex];
            
            asset_id GlyphID = AddAssetGlyph(Storage, 
                                             FontSizeAsset->GUID,
                                             GlyphIndex,
                                             Glyph);
            
            if (GlyphIndex == 0)
            {
                FontSizeHeader->FirstGlyphId = GlyphID;
            }
        }
    }
    
    // NOTE(Dima): Adding font size's glyphs's styles
    for (int FontSizeIndex = 0;
         FontSizeIndex < Font->NumSizes;
         FontSizeIndex++)
    {
        asset_id FontSizeID = Header->FirstFontSizeID + FontSizeIndex;
        
        asset* FontSizeAsset = GetAssetByID(Storage, FontSizeID);
        font_size* FontSize = &Font->Sizes[FontSizeIndex];
        
        asset_header_font_size* FontSizeHeader = FontSizeAsset->Header.FontSize;
        
        for (int GlyphIndex = 0;
             GlyphIndex < Font->NumGlyphs;
             GlyphIndex++)
        {
            asset_id GlyphID = FontSizeHeader->FirstGlyphId + GlyphIndex;
            
            glyph* Glyph = &FontSize->Glyphs[GlyphIndex];
            asset* GlyphAsset = GetAssetByID(Storage, GlyphID);
            asset_header_glyph* GlyphHeader = GlyphAsset->Header.Glyph;
            
            for (int StyleIndex = 0;
                 StyleIndex < Glyph->NumStyles;
                 StyleIndex++)
            {
                glyph_style* Style = &Glyph->Styles[StyleIndex];
                
                asset_id StyleID = AddAssetGlyphStyle(Storage, 
                                                      GlyphAsset->GUID,
                                                      StyleIndex,
                                                      Style);
                
                if (StyleIndex == 0)
                {
                    GlyphHeader->FirstStyleID = StyleID;
                }
            }
        }
    }
    
    return Result;
}
