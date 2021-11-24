#include "flower_utils.cpp"
#include "flower_software_render.cpp"

#include <array>

INTERNAL_FUNCTION image* LoadImageFile(char* FilePath, 
                                       const loading_params& Params = LoadingParams_Image())
{
    int Width;
    int Height;
    int Channels;
    
    b32 FlipVert = true;
    if(Params.Image.FlipVertically)
    {
        FlipVert = false;
    }
    
    stbi_set_flip_vertically_on_load(FlipVert);
    
    unsigned char* LoadedImageBytes = stbi_load(FilePath,
                                                &Width,
                                                &Height,
                                                &Channels,
                                                STBI_rgb_alpha);
    
    Assert(LoadedImageBytes);
    
    image* Result = 0;
    if (LoadedImageBytes)
    {
        int PixCount = Width * Height;
        int PixelsSize = PixCount * 4;
        mi OffsetToPixelsData = Align(sizeof(image), 64);
        
        void* ResultMem = malloc(OffsetToPixelsData + PixelsSize);
        
        Result = (image*)ResultMem;
        void* ResultPixels = (u8*)ResultMem + OffsetToPixelsData;
        
        AllocateImageInternal(Result, Width, Height, ResultPixels);
        Result->FilteringIsClosest = Params.Image.FilteringIsClosest;
        
        Result->Align = Params.Image.Align;
        
        for(int PixelIndex = 0;
            PixelIndex < PixCount;
            PixelIndex++)
        {
            u32 Pix = *((u32*)LoadedImageBytes + PixelIndex);
            
            v4 Color = PremultiplyAlpha(UnpackRGBA(Pix));
            
            u32 PackedColor = PackRGBA(Color);
            
            *((u32*)Result->Pixels + PixelIndex) = PackedColor;
        }
        
        stbi_image_free(LoadedImageBytes);
    }
    
    return(Result);
}

INTERNAL_FUNCTION cubemap* LoadCubemap(const char* Left,
                                       const char* Right,
                                       const char* Front,
                                       const char* Back,
                                       const char* Up,
                                       const char* Down)
{
    cubemap* Result = (cubemap*)malloc(sizeof(cubemap));
    
    Result->Left = LoadImageFile((char*)Left);
    Result->Right = LoadImageFile((char*)Right);
    Result->Front = LoadImageFile((char*)Front);
    Result->Back = LoadImageFile((char*)Back);
    Result->Top = LoadImageFile((char*)Up);
    Result->Down = LoadImageFile((char*)Down);
    
    return(Result);
}

struct load_font_size_context
{
    u32 SizeTypeEnum;
    
    f32 Scale;
    
    f32 PixelsPerMeter;
    
    std::vector<glyph> Glyphs;
};

struct load_font_context
{
    u8* FileBufferToFree;
    
    f32 Ascent;
    f32 Descent;
    f32 LineGap;
    f32 LineAdvance;
    
    int NumGlyphs;
    font* Result;
    stbtt_fontinfo StbFont;
    
    std::array<std::vector<font_slot_glyph_id>, FONT_MAPPING_SIZE> Mapping;
    
    v2 FontAtlasAtP;
    int FontAtlasMaxRowY;
    
    char UniqueName[256];
    u32 UniqueNameHash;
    
    std::vector<load_font_size_context> FontSizes;
    
    loading_params Params;
};

INTERNAL_FUNCTION std::vector<u32> ExtractFontSizesToLoad(u32 SizesFlags)
{
    std::vector<u32> Result;
    
    for (int i = 0; i < FontSize_Count; i++)
    {
        Result.push_back(i);
    }
    
    return Result;
}

INTERNAL_FUNCTION bool
BeginFontLoading(load_font_context* Ctx, char* FilePath, 
                 loading_params Params)
{
    bool Result = false;
    
    u8* Buffer = (u8*)StandardReadFile(FilePath, 0);
    
    Ctx->Params = Params;
    
    if(Buffer)
    {
        Result = true;
        
        Ctx->FileBufferToFree = Buffer;
        
        stbtt_InitFont(&Ctx->StbFont, Ctx->FileBufferToFree, 0);
        
        int FontNameLength;
        const char * FontName = stbtt_GetFontNameString(&Ctx->StbFont, 
                                                        &FontNameLength, 
                                                        STBTT_PLATFORM_ID_MICROSOFT, 
                                                        STBTT_MS_EID_UNICODE_BMP,
                                                        STBTT_MS_LANG_ENGLISH, 
                                                        4);
        
        char Buf[256];
        for (int i = 0; i < FontNameLength; i++)
        {
            Buf[i] = FontName[i*2 + 1];
        }
        Buf[FontNameLength] = 0;
        
        CopyStringsSafe(Ctx->UniqueName, ArrLen(Ctx->UniqueName), Buf);
        Ctx->UniqueNameHash = StringHashFNV(Ctx->UniqueName);
        
        std::vector<u32> SizesToLoad = ExtractFontSizesToLoad(Params.Font.SizesFlags);
        
        for (u32 FontSizeIndex : SizesToLoad)
        {
            load_font_size_context FontSize = {};
            
            FontSize.SizeTypeEnum = FontSizeIndex;
            
            int PixelHeight = Global_FontSizes[FontSizeIndex];
            FontSize.Scale = stbtt_ScaleForPixelHeight(&Ctx->StbFont, 
                                                       PixelHeight);
            
            FontSize.PixelsPerMeter = PixelHeight;
            
            Ctx->FontSizes.push_back(FontSize);
        }
    }
    
    return Result;
}


INTERNAL_FUNCTION void AddGlyphToAtlas(load_font_context* Ctx, glyph* Glyph)
{
    font* Font = Ctx->Result;
    image* Dst = Font->Atlas;
    
    int DstSize = Dst->Width;
    f32 OneOverSize = 1.0f / (f32)DstSize;
    
    for(int StyleIndex = 0; 
        StyleIndex < FontStyle_Count;
        StyleIndex++)
    {
        glyph_style* Style = &Glyph->Styles[StyleIndex];
        
        // NOTE(Dima): Getting image 
        image* Src = Style->Image;
        if(Src)
        {
            int SrcW = Src->Width;
            int SrcH = Src->Height;
            
            int DstPx = std::ceil(Ctx->FontAtlasAtP.x);
            int DstPy = std::ceil(Ctx->FontAtlasAtP.y);
            
            if(DstPx + SrcW >= DstSize)
            {
                DstPx = 0;
                DstPy = Ctx->FontAtlasMaxRowY;
            }
            
            Assert(DstPy + SrcH < DstSize);
            
            Style->MinUV = V2(DstPx, DstPy) * OneOverSize;
            Style->MaxUV = V2(DstPx + SrcW, DstPy + SrcH) * OneOverSize;
            
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
            
            Ctx->FontAtlasAtP = V2(DstPx + SrcW, DstPy);
            Ctx->FontAtlasMaxRowY = FlowerMax(Ctx->FontAtlasMaxRowY, DstPy + SrcH);
        }
    }
}

INTERNAL_FUNCTION void 
EndFontLoading(load_font_context* Ctx)
{
    int NumGlyphs = Ctx->NumGlyphs;
    int NumSizes = Ctx->FontSizes.size();
    int AtlasWidth = Ctx->Params.Font.AtlasWidth;
    
    // NOTE(Dima): Copying glyphs
    helper_byte_buffer HelpBytes;
    HelpBytes.AddPlace("FontResultPtr", 1, sizeof(font));
    
    for (int SizeIndex = 0;
         SizeIndex < NumSizes;
         SizeIndex++)
    {
        char TempBuf[256];
        
        stbsp_sprintf(TempBuf, "Ptrs[%d]", SizeIndex);
        HelpBytes.AddPlace(TempBuf, NumGlyphs, sizeof(glyph*));
        
        stbsp_sprintf(TempBuf, "Glyphs[%d]", SizeIndex);
        HelpBytes.AddPlace(TempBuf, NumGlyphs, sizeof(glyph));
    }
    
    HelpBytes.AddPlace("Kerning", NumGlyphs * NumGlyphs, sizeof(f32));
    HelpBytes.AddPlace("CodepointToSlot", FONT_MAPPING_SIZE, sizeof(font_codepoint_slot_range));
    HelpBytes.AddPlace("SlotsGlyphsIds", NumGlyphs, sizeof(font_slot_glyph_id));
    HelpBytes.AddPlace("AtlasImage", 1, sizeof(image));
    HelpBytes.AddPlace("AtlasData", AtlasWidth * AtlasWidth, 4);
    HelpBytes.AddPlace("FontSizes", NumSizes, sizeof(font_size));
    
    HelpBytes.Generate();
    
    Ctx->Result = (font*)HelpBytes.GetPlace("FontResultPtr");
    font* Result = Ctx->Result;
    
    // NOTE(Dima): SEtting some result fields
    CopyStringsSafe(Result->UniqueName,
                    ArrLen(Result->UniqueName),
                    Ctx->UniqueName);
    Result->UniqueNameHash = StringHashFNV(Ctx->UniqueName);
    Result->NumGlyphs = NumGlyphs;
    Result->NumSizes = NumSizes;
    Result->Sizes = (font_size*)HelpBytes.GetPlace("FontSizes");
    
    // NOTE(Dima): Getting font metrics
    int StbAscent;
    int StbDescent;
    int StbLineGap;
    
    stbtt_GetFontVMetrics(&Ctx->StbFont, 
                          &StbAscent, 
                          &StbDescent, 
                          &StbLineGap);
    
    Result->Ascent = (f32)StbAscent;
    Result->Descent = (f32)StbDescent;
    Result->LineGap = (f32)StbLineGap;
    Result->LineAdvance = Result->Ascent - Result->Descent + Result->LineGap;
    
    for (int SizeIndex = 0;
         SizeIndex < NumSizes;
         SizeIndex++)
    {
        load_font_size_context* SizeCtx = &Ctx->FontSizes[SizeIndex];
        font_size* FontSize = &Result->Sizes[SizeIndex];
        
        FontSize->ScaleForPixelHeight = SizeCtx->Scale;
        FontSize->FontSizeEnumType = SizeCtx->SizeTypeEnum;
        FontSize->PixelsPerMeter = SizeCtx->PixelsPerMeter;
        
        // NOTE(Dima): Getting glyphs
        char TempBuf[256];
        
        stbsp_sprintf(TempBuf, "Glyphs[%d]", SizeIndex);
        glyph* GlyphsArr = (glyph*)HelpBytes.GetPlace(TempBuf);
        
        stbsp_sprintf(TempBuf, "Ptrs[%d]", SizeIndex);
        FontSize->Glyphs = (glyph**)HelpBytes.GetPlace(TempBuf);
        
        // NOTE(Dima): Copying glyphs ptrs
        memcpy(GlyphsArr, &SizeCtx->Glyphs[0], NumGlyphs * sizeof(glyph));
        
        for (int GlyphIndex = 0;
             GlyphIndex < NumGlyphs;
             GlyphIndex++)
        {
            FontSize->Glyphs[GlyphIndex] = &GlyphsArr[GlyphIndex];
        }
    }
    
    Result->KerningPairs = (f32*)HelpBytes.GetPlace("Kerning");
    Result->CodepointToSlot = (font_codepoint_slot_range*)HelpBytes.GetPlace("CodepointToSlot");
    Result->SlotsGlyphsIds = (font_slot_glyph_id*)HelpBytes.GetPlace("SlotsGlyphsIds");
    Result->Atlas = (image*)HelpBytes.GetPlace("AtlasImage");
    
    
    // NOTE(Dima): Integrating glyph mapping
    std::vector<font_slot_glyph_id> SlotsGlyphsIds;
    std::vector<font_codepoint_slot_range> CodepointToSlot; 
    for (int i = 0; i < FONT_MAPPING_SIZE; i++)
    {
        const vector<font_slot_glyph_id>& CurVector = Ctx->Mapping[i];
        
        font_codepoint_slot_range NewRange = {};
        NewRange.Count = CurVector.size();
        NewRange.StartIndexInSlotsGlyphsIds = SlotsGlyphsIds.size();
        
        for (font_slot_glyph_id GlyphIndex : CurVector)
        {
            SlotsGlyphsIds.push_back(GlyphIndex);
        }
        
        CodepointToSlot.push_back(NewRange);
    }
    
    memcpy(Result->CodepointToSlot, &CodepointToSlot[0],
           sizeof(font_codepoint_slot_range) * FONT_MAPPING_SIZE);
    memcpy(Result->SlotsGlyphsIds, &SlotsGlyphsIds[0], 
           sizeof(font_slot_glyph_id) * NumGlyphs);
    
    // NOTE(Dima): Loading kerning
    Assert(NumSizes > 0);
    load_font_size_context* FirstSizeCtx = &Ctx->FontSizes[0];
    for(int i = 0; i < NumGlyphs; i++)
    {
        for(int j = 0; j < NumGlyphs; j++)
        {
            int Index = i * NumGlyphs + j;
            
            u32 A = FirstSizeCtx->Glyphs[i].Codepoint;
            u32 B = FirstSizeCtx->Glyphs[j].Codepoint;
            
            int ExtractedKern = stbtt_GetCodepointKernAdvance(&Ctx->StbFont, A, B);
            
            Result->KerningPairs[Index] = (f32)ExtractedKern;
        }
    }
    
    // NOTE(Dima): Placing glyphs characters into font atlas
    memset(Result->Atlas, 0, sizeof(image));
    AllocateImageInternal(Result->Atlas, 
                          Ctx->Params.Font.AtlasWidth,
                          Ctx->Params.Font.AtlasWidth,
                          HelpBytes.GetPlace("AtlasData"));
    ClearImage(Result->Atlas);
    
    for (int SizeIndex = 0;
         SizeIndex < Result->NumSizes;
         SizeIndex++)
    {
        font_size* FontSize = &Result->Sizes[SizeIndex];
        
        for(int GlyphIndex = 0;
            GlyphIndex < NumGlyphs;
            GlyphIndex++)
        {
            glyph* Glyph = FontSize->Glyphs[GlyphIndex];
            
            AddGlyphToAtlas(Ctx, Glyph);
        }
    }
    
    // NOTE(Dima): Freeing file buffer
    if(Ctx->FileBufferToFree)
    {
        free(Ctx->FileBufferToFree);
    }
    Ctx->FileBufferToFree = 0;
}

INTERNAL_FUNCTION void AddGlyphToFont(load_font_context* Ctx, u32 Codepoint)
{
    glyph Glyph = {};
    
    int SlotIndex = Codepoint % FONT_MAPPING_SIZE;
    
    font_slot_glyph_id SlotGlyphId = {};
    SlotGlyphId.Codepoint = Codepoint;
    SlotGlyphId.IndexInGlyphs = Ctx->NumGlyphs++;
    Ctx->Mapping[SlotIndex].push_back(SlotGlyphId);
    
    int StbAdvance;
    int StbLeftBearing;
    stbtt_GetCodepointHMetrics(&Ctx->StbFont, 
                               Codepoint, 
                               &StbAdvance, 
                               &StbLeftBearing);
    
    for (int SizeIndex = 0;
         SizeIndex < Ctx->FontSizes.size();
         SizeIndex++)
    {
        load_font_size_context* SizeCtx = &Ctx->FontSizes[SizeIndex];
        
        int StbW, StbH;
        int StbXOffset, StbYOffset;
        unsigned char* StbBitmap = stbtt_GetCodepointBitmap(&Ctx->StbFont,
                                                            0, SizeCtx->Scale,
                                                            Codepoint,
                                                            &StbW, &StbH,
                                                            &StbXOffset, 
                                                            &StbYOffset);
        b32 ImageExist = true;
        
        if(StbW > 20000)
        {
            StbW = 0;
            ImageExist = false;
        }
        
        if(StbH > 20000)
        {
            StbH = 0;
            ImageExist = false;
        }
        
        if(!StbBitmap && (Codepoint != ' '))
        {
            ImageExist = false;
        }
        
        glyph_style* Styles = Glyph.Styles;
        if(ImageExist)
        {
            image* StbImage = AllocateImage(StbW, StbH);
            
            // NOTE(Dima): Initializing stb image
            ConvertGrayscaleToRGBA(StbImage, StbBitmap);
            
            b32 ShouldPremultiplyAlpha = true;
            
            int Border = 3;
            int ShadowOffset = Ctx->Params.Font.ShadowOffset;
            
            // NOTE(Dima): Creating image for regular font
            int RegularWidth = StbW + 2 * Border;
            int RegularHeight = StbH + 2 * Border;
            image* RegularImage = AllocateImage(RegularWidth, RegularHeight);
            
            Styles[FontStyle_Regular] = CreateGlyphStyle(RegularImage, 
                                                         RegularWidth, 
                                                         RegularHeight);
            
            RenderOneBitmapIntoAnother(RegularImage,
                                       StbImage,
                                       Border, Border,
                                       ColorWhite());
            
            // NOTE(Dima): Creating image for shadowed font
            int ImageShadowWidth = RegularWidth + ShadowOffset;
            int ImageShadowHeight = RegularHeight + ShadowOffset;
            image* ShadowImage = AllocateImage(ImageShadowWidth, ImageShadowHeight);
            
            Styles[FontStyle_Shadow] = CreateGlyphStyle(ShadowImage,
                                                        ImageShadowWidth,
                                                        ImageShadowHeight);
            
            RenderOneBitmapIntoAnother(ShadowImage,
                                       StbImage,
                                       Border + ShadowOffset,
                                       Border + ShadowOffset,
                                       Ctx->Params.Font.ShadowColor);
            
            RenderOneBitmapIntoAnother(ShadowImage,
                                       StbImage,
                                       Border, Border,
                                       ColorWhite());
            
            // NOTE(Dima): Creating image for outlined font
            int ImageOutlineW = RegularWidth;
            int ImageOutlineH = RegularHeight;
            image* OutlineSrc = AllocateImage(ImageOutlineW, ImageOutlineH);
            image* OutlineImage = AllocateImage(ImageOutlineW, ImageOutlineH);
            
            Styles[FontStyle_Outline] = CreateGlyphStyle(OutlineImage,
                                                         ImageOutlineW,
                                                         ImageOutlineH);
            
#if 0       
            RenderOneBitmapIntoAnother(&OutlineSrc,
                                       &StbImage,
                                       Border,
                                       Border,
                                       Params.Font_OutlineColor);
            
            image* OutlineTemp = AllocateImage(ImageOutlineW, ImageOutlineH);
            BlurBitmapApproximateGaussian(OutlineImage,
                                          OutlineSrc,
                                          OutlineTemp,
                                          3);
            
            for(int y = 0; y < OutlineImage.Height; y++)
            {
                for(int x = 0; x < OutlineImage.Width; x++)
                {
                    u32* At = (u32*)OutlineImage.Pixels + OutlineImage.Width * y + x;
                    
                    v4 Color = UnpackRGBA(*At);
                    
                    if(Color.a > 0.1f)
                    {
                        Color = Params.Font_OutlineColor;
                    }
                    
                    *At = PackRGBA(Color);
                }
            }
#else
            // NOTE(Dima): Simple cellural automata for outline generation
            int CellDist = 1;
            RenderOneBitmapIntoAnother(OutlineImage,
                                       StbImage,
                                       Border,
                                       Border,
                                       Ctx->Params.Font.OutlineColor);
            
            for(int y = CellDist; y < OutlineImage->Height - CellDist; y++)
            {
                for(int x = CellDist; x < OutlineImage->Width - CellDist; x++)
                {
                    u32* At = (u32*)OutlineSrc->Pixels + OutlineImage->Width * y + x;
                    
                    f32 NearAlphaSum = 0.0f;
                    for(int CheckY = y - CellDist; CheckY <= y + CellDist; CheckY++)
                    {
                        for(int CheckX = x - CellDist; CheckX <= x + CellDist; CheckX++)
                        {
                            u32* CheckAt = (u32*)OutlineImage->Pixels + OutlineImage->Width * CheckY + CheckX;
                            
                            v4 CheckColor = UnpackRGBA(*CheckAt);
                            
                            NearAlphaSum += CheckColor.a;
                        }
                    }
                    
                    v4 Color = ColorClear();
                    if(NearAlphaSum >= 1.0f)
                    {
                        Color = Ctx->Params.Font.OutlineColor;
                    }
                    
                    *At = PackRGBA(Color);
                }
            }
            
            image* OutlineTemp = AllocateImage(ImageOutlineW, ImageOutlineH);
            BlurBitmapApproximateGaussian(OutlineImage,
                                          OutlineSrc,
                                          OutlineTemp,
                                          2);
#endif
            
            RenderOneBitmapIntoAnother(OutlineImage,
                                       StbImage,
                                       Border,
                                       Border,
                                       ColorWhite());
            
            // NOTE(Dima): Setting other glyph information
            Glyph.Codepoint = Codepoint;
            Glyph.Advance = (f32)StbAdvance * SizeCtx->Scale;
            Glyph.LeftBearing = (f32)StbLeftBearing * SizeCtx->Scale;
            
            Glyph.XOffset = StbXOffset - Border;
            Glyph.YOffset = StbYOffset - Border;
        }
        else
        {
            // TODO(Dima): Set image indices to invalid
            for(int StyleIndex = 0;
                StyleIndex < FontStyle_Count;
                StyleIndex++)
            {
                glyph_style* Style = Styles + StyleIndex;
                
                *Style = CreateGlyphStyle(nullptr, 1, 1);
            }
        }
        
        if(StbBitmap)
        {
            stbtt_FreeBitmap(StbBitmap, 0);
        }
        
        SizeCtx->Glyphs.push_back(Glyph);
    }
}

INTERNAL_FUNCTION font* LoadFontFile(char* FilePath, 
                                     const loading_params& Params = LoadingParams_Font())
{
    load_font_context Ctx = {};
    
    if (BeginFontLoading(&Ctx, FilePath, Params))
    {
        for(int Codepoint = ' ';
            Codepoint <= '~';
            Codepoint++)
        {
            AddGlyphToFont(&Ctx, Codepoint);
        }
        
        EndFontLoading(&Ctx);
    }
    
    return(Ctx.Result);
}

INTERNAL_FUNCTION mesh* MakeMesh(const helper_mesh& HelperMesh)
{
    helper_byte_buffer Help = {};
    
    render_mesh_offsets Offsets = {};
    Offsets.OffsetP = Help.AddPlace("P", HelperMesh.Vertices.size(), sizeof(v3));
    Offsets.OffsetUV = Help.AddPlace("UV", HelperMesh.TexCoords.size(), sizeof(v2));
    Offsets.OffsetN = Help.AddPlace("N", HelperMesh.Normals.size(), sizeof(v3));
    Offsets.OffsetC = Help.AddPlace("C", HelperMesh.Colors.size(), sizeof(u32));
    Offsets.OffsetBoneWeights = Help.AddPlace("BoneWeights", HelperMesh.BoneWeights.size(), sizeof(v4));
    Offsets.OffsetBoneIndices = Help.AddPlace("BoneIndices", HelperMesh.BoneIndices.size(), sizeof(u32));
    
    // NOTE(Dima): Determining data size
    mi MeshDataSize = Help.DataSize;
    
    Help.AddPlace("Indices", HelperMesh.Indices.size(), sizeof(u32));
    Help.AddPlace("ResultPtr", 1, sizeof(mesh));
    
    Help.Generate();
    
    mesh* Result = (mesh*)Help.GetPlace("ResultPtr");
    
    Result->P = (v3*)Help.GetPlace("P");
    Result->UV = (v2*)Help.GetPlace("UV");
    Result->N = (v3*)Help.GetPlace("N");
    Result->C = (u32*)Help.GetPlace("C");
    Result->BoneWeights = (v4*)Help.GetPlace("BoneWeights");
    Result->BoneIndices = (u32*)Help.GetPlace("BoneIndices");
    
    Result->Offsets = Offsets;
    Result->Handle = {};
    
    // NOTE(Dima): Copy name
    const char* SrcName = HelperMesh.Name.c_str();
    CopyStringsSafe(Result->Name, ArrLen(Result->Name), (char*)SrcName);
    
    // NOTE(Dima): Saving vertices data buffer
    Result->MeshDataStart = Result->P;
    Result->MeshDataSize = MeshDataSize;
    Result->VertexCount = HelperMesh.Vertices.size();
    Result->IsSkinned = HelperMesh.IsSkinned;
    
    // NOTE(Dima): Allocating indices
    Result->Indices = (u32*)Help.GetPlace("Indices");;
    Result->IndexCount = HelperMesh.Indices.size();
    
    // NOTE(Dima): Storing vertex data
    for(int VertexIndex = 0;
        VertexIndex < HelperMesh.Vertices.size();
        VertexIndex++)
    {
        Result->P[VertexIndex] = HelperMesh.Vertices[VertexIndex];
        Result->UV[VertexIndex] = HelperMesh.TexCoords[VertexIndex];
        Result->N[VertexIndex] = HelperMesh.Normals[VertexIndex];
        Result->C[VertexIndex] = PackRGB(HelperMesh.Colors[VertexIndex]);
        if(HelperMesh.IsSkinned)
        {
            Result->BoneWeights[VertexIndex] = HelperMesh.BoneWeights[VertexIndex];
            Result->BoneIndices[VertexIndex] = HelperMesh.BoneIndices[VertexIndex];
        }
    }
    
    for (int Index = 0; Index < HelperMesh.Indices.size(); Index += 1)
    {
        Result->Indices[Index] = HelperMesh.Indices[Index];
    }
    
    return(Result);
}

INTERNAL_FUNCTION inline void PushTriangle(helper_mesh& HelperMesh,
                                           v3 A, v3 B, v3 C,
                                           v3 N, v3 Color)
{
    int VertexBase = HelperMesh.Vertices.size();
    
    auto& Vertices = HelperMesh.Vertices;
    auto& TexCoords = HelperMesh.TexCoords;
    auto& Normals = HelperMesh.Normals;
    auto& Colors = HelperMesh.Colors;
    auto& Indices = HelperMesh.Indices;
    
    Vertices.push_back(A);
    Vertices.push_back(B);
    Vertices.push_back(C);
    
    TexCoords.push_back(V2(0.0f, 1.0f));
    TexCoords.push_back(V2(1.0f, 1.0f));
    TexCoords.push_back(V2(1.0f, 0.0f));
    
    Normals.push_back(N);
    Normals.push_back(N);
    Normals.push_back(N);
    
    Colors.push_back(Color);
    Colors.push_back(Color);
    Colors.push_back(Color);
    
    Indices.push_back(VertexBase + 0);
    Indices.push_back(VertexBase + 1);
    Indices.push_back(VertexBase + 2);
}

INTERNAL_FUNCTION inline void PushFlatPolygon(helper_mesh& HelperMesh,
                                              v3 A, v3 B, v3 C, v3 D,
                                              v3 N, v3 Color)
{
    int VertexBase = HelperMesh.Vertices.size();
    
    auto& Vertices = HelperMesh.Vertices;
    auto& TexCoords = HelperMesh.TexCoords;
    auto& Normals = HelperMesh.Normals;
    auto& Colors = HelperMesh.Colors;
    auto& Indices = HelperMesh.Indices;
    
    Vertices.push_back(A);
    Vertices.push_back(B);
    Vertices.push_back(C);
    Vertices.push_back(D);
    
    TexCoords.push_back(V2(0.0f, 1.0f));
    TexCoords.push_back(V2(1.0f, 1.0f));
    TexCoords.push_back(V2(1.0f, 0.0f));
    TexCoords.push_back(V2(0.0f, 0.0f));
    
    Normals.push_back(N);
    Normals.push_back(N);
    Normals.push_back(N);
    Normals.push_back(N);
    
    Colors.push_back(Color);
    Colors.push_back(Color);
    Colors.push_back(Color);
    Colors.push_back(Color);
    
    Indices.push_back(VertexBase + 0);
    Indices.push_back(VertexBase + 1);
    Indices.push_back(VertexBase + 2);
    Indices.push_back(VertexBase + 0);
    Indices.push_back(VertexBase + 2);
    Indices.push_back(VertexBase + 3);
}

INTERNAL_FUNCTION inline void PushUnitCubeSide(helper_mesh& HelperMesh,
                                               int Index0, 
                                               int Index1,
                                               int Index2,
                                               int Index3,
                                               v3 Normal,
                                               v3 Color,
                                               f32 SideLen)
{
    f32 HalfSideLen = SideLen * 0.5f;
    
    v3 CubeVertices[8] =
    {
        HalfSideLen * V3(-1.0f, 1.0f, 1.0f),
        HalfSideLen * V3(1.0f, 1.0f, 1.0f),
        HalfSideLen * V3(1.0f, -1.0f, 1.0f),
        HalfSideLen * V3(-1.0f, -1.0f, 1.0f),
        
        HalfSideLen * V3(-1.0f, 1.0f, -1.0f),
        HalfSideLen * V3(1.0f, 1.0f, -1.0f),
        HalfSideLen * V3(1.0f, -1.0f, -1.0f),
        HalfSideLen * V3(-1.0f, -1.0f, -1.0f),
    };
    
    PushFlatPolygon(HelperMesh,
                    CubeVertices[Index0],
                    CubeVertices[Index1],
                    CubeVertices[Index2],
                    CubeVertices[Index3],
                    Normal, Color);
}

INTERNAL_FUNCTION mesh* MakeUnitCube(f32 SideLen = 1.0f)
{
    helper_mesh HelperMesh = {};
    
    v3 CubeColor = V3(1.0f);
    
    // NOTE(Dima): Forward side
    PushUnitCubeSide(HelperMesh,
                     0, 1, 2, 3, 
                     V3_Forward(), 
                     CubeColor,
                     SideLen);
    
    // NOTE(Dima): Left side
    PushUnitCubeSide(HelperMesh,
                     1, 5, 6, 2, 
                     V3_Left(), 
                     CubeColor,
                     SideLen);
    
    // NOTE(Dima): Back side
    PushUnitCubeSide(HelperMesh,
                     5, 4, 7, 6, 
                     V3_Back(), 
                     CubeColor,
                     SideLen);
    
    // NOTE(Dima): Right side
    PushUnitCubeSide(HelperMesh,
                     4, 0, 3, 7, 
                     V3_Right(), 
                     CubeColor,
                     SideLen);
    
    // NOTE(Dima): Up
    PushUnitCubeSide(HelperMesh,
                     4, 5, 1, 0, 
                     V3_Up(), 
                     CubeColor,
                     SideLen);
    
    // NOTE(Dima): Down
    PushUnitCubeSide(HelperMesh,
                     3, 2, 6, 7, 
                     V3_Down(), 
                     CubeColor,
                     SideLen);
    
    mesh* Result = MakeMesh(HelperMesh);
    
    return(Result);
}

mesh* MakePlane()
{
    helper_mesh HelperMesh = {};
    
    v3 Points[4] = 
    {
        V3(1.0f, 0.0f, 1.0f),
        V3(-1.0f, 0.0f, 1.0f),
        V3(-1.0f, 0.0f, -1.0f),
        V3(1.0f, 0.0f, -1.0f)
    };
    
    PushFlatPolygon(HelperMesh,
                    Points[0], Points[1],
                    Points[2], Points[3],
                    V3_Up(), V3_One());
    
    mesh* Result = MakeMesh(HelperMesh);
    
    return(Result);
}

#include "asset_tool_assimp.cpp"