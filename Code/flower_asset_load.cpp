#include "flower_utils.cpp"
#include "flower_software_render.cpp"

#include <array>

struct loading_params
{
    u32 AssetType;
    
    b32 Image_FlipVertically;
    b32 Image_FilteringIsClosest;
    
    f32 Model_DefaultScale;
    b32 Model_FixInvalidRotation;
    
    int Font_PixelHeight;
    v4 Font_ShadowColor;
    v4 Font_OutlineColor;
    int Font_ShadowOffset;
};

inline loading_params DefaultLoadingParams()
{
    loading_params Result = {};
    
    // NOTE(Dima): Image
    Result.Image_FlipVertically = true;
    Result.Image_FilteringIsClosest = false;
    
    // NOTE(Dima): Model
    Result.Model_FixInvalidRotation = false;
    Result.Model_DefaultScale = 1.0f;
    
    // NOTE(Dima): Font
    Result.Font_PixelHeight = 30;
    Result.Font_ShadowOffset = 1;
    Result.Font_ShadowColor = ColorBlack();
    Result.Font_OutlineColor = ColorBlack();
    
    return(Result);
}

INTERNAL_FUNCTION image* LoadImageFile(char* FilePath, 
                                       const loading_params& Params = DefaultLoadingParams())
{
    int Width;
    int Height;
    int Channels;
    
    b32 FlipVert = true;
    if(Params.Image_FlipVertically)
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
        Result->FilteringIsClosest = Params.Image_FilteringIsClosest;
        
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

INTERNAL_FUNCTION cubemap LoadCubemap(const char* Left,
                                      const char* Right,
                                      const char* Front,
                                      const char* Back,
                                      const char* Up,
                                      const char* Down)
{
    cubemap Result = {};
    
    Result.Left = LoadImageFile((char*)Left);
    Result.Right = LoadImageFile((char*)Right);
    Result.Front = LoadImageFile((char*)Front);
    Result.Back = LoadImageFile((char*)Back);
    Result.Top = LoadImageFile((char*)Up);
    Result.Down = LoadImageFile((char*)Down);
    
    return(Result);
}

INTERNAL_FUNCTION font LoadFontFromBuffer(u8* Buffer, 
                                          const loading_params& Params)
{
    font Result = {};
    
    Result.PixelsPerMeter = Params.Font_PixelHeight;
    
    stbtt_fontinfo StbFont;
    stbtt_InitFont(&StbFont, Buffer, 0);
    
    f32 Scale = stbtt_ScaleForPixelHeight(&StbFont, Params.Font_PixelHeight);
    
    int StbAscent;
    int StbDescent;
    int StbLineGap;
    stbtt_GetFontVMetrics(&StbFont, &StbAscent, &StbDescent, &StbLineGap);
    
    Result.Ascent = (f32)StbAscent * Scale;
    Result.Descent = (f32)StbDescent * Scale;
    Result.LineGap = (f32)StbLineGap * Scale;
    
    Result.LineAdvance = Result.Ascent - Result.Descent + Result.LineGap;
    
    std::vector<glyph> Glyphs;
    std::array<std::vector<font_slot_glyph_id>, FONT_MAPPING_SIZE> Mapping;
    
    int GlyphCount = ('~' - ' ' + 1);
    
    for(int Codepoint = ' ';
        Codepoint <= '~';
        Codepoint++)
    {
        glyph Glyph = {};
        
        int SlotIndex = Codepoint % FONT_MAPPING_SIZE;
        
        font_slot_glyph_id SlotGlyphId = {};
        SlotGlyphId.Codepoint = Codepoint;
        SlotGlyphId.IndexInGlyphs = Glyphs.size();
        Mapping[SlotIndex].push_back(SlotGlyphId);
        
        int StbAdvance;
        int StbLeftBearing;
        stbtt_GetCodepointHMetrics(&StbFont, 
                                   Codepoint, 
                                   &StbAdvance, 
                                   &StbLeftBearing);
        
        int StbW, StbH;
        int StbXOffset, StbYOffset;
        unsigned char* StbBitmap = stbtt_GetCodepointBitmap(&StbFont,
                                                            0, Scale,
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
            int ShadowOffset = Params.Font_ShadowOffset;
            
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
                                       Params.Font_ShadowColor);
            
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
                                       Params.Font_OutlineColor);
            
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
                        Color = Params.Font_OutlineColor;
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
            Glyph.Advance = (f32)StbAdvance * Scale;
            Glyph.LeftBearing = (f32)StbLeftBearing * Scale;
            
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
        
        Glyphs.push_back(Glyph);
    }
    
    // NOTE(Dima): Copying glyphs
    helper_byte_buffer HelpBytes;
    HelpBytes.AddPlace("Ptrs", Glyphs.size(), sizeof(glyph*));
    HelpBytes.AddPlace("Glyphs", Glyphs.size(), sizeof(glyph));
    HelpBytes.Generate();
    
    glyph* GlyphsArr = (glyph*)HelpBytes.GetPlace("Glyphs");
    glyph** Ptrs = (glyph**)HelpBytes.GetPlace("Ptrs");
    
    memcpy(GlyphsArr, &Glyphs[0], Glyphs.size() * sizeof(glyph));
    
    for (int GlyphIndex = 0;
         GlyphIndex < Glyphs.size();
         GlyphIndex++)
    {
        Ptrs[GlyphIndex] = &GlyphsArr[GlyphIndex];
    }
    
    Result.Glyphs = Ptrs;
    Result.GlyphCount = Glyphs.size();
    
    
    // NOTE(Dima): Integrating glyph mapping
    std::vector<font_slot_glyph_id> SlotsGlyphsIds;
    std::vector<font_codepoint_slot_range> CodepointToSlot; 
    for (int i = 0; i < FONT_MAPPING_SIZE; i++)
    {
        const vector<font_slot_glyph_id>& CurVector = Mapping[i];
        
        font_codepoint_slot_range NewRange = {};
        NewRange.Count = CurVector.size();
        NewRange.StartIndexInSlotsGlyphsIds = SlotsGlyphsIds.size();
        
        for (font_slot_glyph_id GlyphIndex : CurVector)
        {
            SlotsGlyphsIds.push_back(GlyphIndex);
        }
        
        CodepointToSlot.push_back(NewRange);
    }
    
    size_t SlotMappingSize = sizeof(font_codepoint_slot_range) * FONT_MAPPING_SIZE;
    size_t SlotsGlyphsIdsSize = sizeof(font_slot_glyph_id) * Glyphs.size();
    Result.CodepointToSlot = (font_codepoint_slot_range*)malloc(SlotMappingSize);
    Result.SlotsGlyphsIds = (font_slot_glyph_id*)malloc(SlotsGlyphsIdsSize);
    
    memcpy(Result.CodepointToSlot, &CodepointToSlot[0], SlotMappingSize);
    memcpy(Result.SlotsGlyphsIds, &SlotsGlyphsIds[0], SlotsGlyphsIdsSize);
    
    
    // NOTE(Dima): Loading kerning
    Result.KerningPairs = (f32*)malloc(Result.GlyphCount * Result.GlyphCount * sizeof(f32));
    
    for(int i = 0; i < Result.GlyphCount; i++)
    {
        for(int j = 0; j < Result.GlyphCount; j++)
        {
            int Index = i * Result.GlyphCount + j;
            
            u32 A = Glyphs[i].Codepoint;
            u32 B = Glyphs[j].Codepoint;
            
            int ExtractedKern = stbtt_GetCodepointKernAdvance(&StbFont, A, B);
            
            if(ExtractedKern != 0)
            {
                int a = 1;
            }
            
            Result.KerningPairs[Index] = Scale * (f32)ExtractedKern;
            //Result.KerningPairs[Index] = 10.0f;
        }
    }
    
    return(Result);
}

INTERNAL_FUNCTION font LoadFontFile(char* FilePath, 
                                    const loading_params& Params = DefaultLoadingParams())
{
    u8* Buffer = (u8*)StandardReadFile(FilePath, 0);
    
    font Result = {};
    if(Buffer)
    {
        Result = LoadFontFromBuffer(Buffer, Params);
    }
    
    free(Buffer);
    
    return(Result);
}

INTERNAL_FUNCTION mesh MakeMesh(const helper_mesh& HelperMesh)
{
    mesh Result = {};
    
    // NOTE(Dima): Copy name
    const char* SrcName = HelperMesh.Name.c_str();
    CopyStringsSafe(Result.Name, ArrayCount(Result.Name), (char*)SrcName);
    
    render_mesh_offsets* Offsets = &Result.Offsets;
    helper_byte_buffer Help = {};
    
    Offsets->OffsetP = Help.AddPlace("P", HelperMesh.Vertices.size(), sizeof(v3));
    Offsets->OffsetUV = Help.AddPlace("UV", HelperMesh.TexCoords.size(), sizeof(v2));
    Offsets->OffsetN = Help.AddPlace("N", HelperMesh.Normals.size(), sizeof(v3));
    Offsets->OffsetC = Help.AddPlace("C", HelperMesh.Colors.size(), sizeof(u32));
    Offsets->OffsetBoneWeights = Help.AddPlace("BoneWeights", HelperMesh.BoneWeights.size(), sizeof(v4));
    Offsets->OffsetBoneIndices = Help.AddPlace("BoneIndices", HelperMesh.BoneIndices.size(), sizeof(u32));
    Help.AddPlace("Indices", HelperMesh.Indices.size(), sizeof(u32));
    
    Help.Generate();
    
    Result.P = (v3*)Help.GetPlace("P");
    Result.UV = (v2*)Help.GetPlace("UV");
    Result.N = (v3*)Help.GetPlace("N");
    Result.C = (u32*)Help.GetPlace("C");
    Result.BoneWeights = (v4*)Help.GetPlace("BoneWeights");
    Result.BoneIndices = (u32*)Help.GetPlace("BoneIndices");
    
    // NOTE(Dima): Saving vertices data buffer
    Result.Free = Help.Data;
    Result.FreeSize = Help.DataSize;
    Result.VertexCount = HelperMesh.Vertices.size();
    Result.IsSkinned = HelperMesh.IsSkinned;
    
    // NOTE(Dima): Allocating indices
    Result.Indices = (u32*)Help.GetPlace("Indices");;
    Result.IndexCount = HelperMesh.Indices.size();
    
    // NOTE(Dima): Storing vertex data
    for(int VertexIndex = 0;
        VertexIndex < HelperMesh.Vertices.size();
        VertexIndex++)
    {
        Result.P[VertexIndex] = HelperMesh.Vertices[VertexIndex];
        Result.UV[VertexIndex] = HelperMesh.TexCoords[VertexIndex];
        Result.N[VertexIndex] = HelperMesh.Normals[VertexIndex];
        Result.C[VertexIndex] = PackRGB(HelperMesh.Colors[VertexIndex]);
        if(HelperMesh.IsSkinned)
        {
            Result.BoneWeights[VertexIndex] = HelperMesh.BoneWeights[VertexIndex];
            Result.BoneIndices[VertexIndex] = HelperMesh.BoneIndices[VertexIndex];
        }
    }
    
    for (int Index = 0; Index < HelperMesh.Indices.size(); Index += 1)
    {
        Result.Indices[Index] = HelperMesh.Indices[Index];
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

INTERNAL_FUNCTION mesh MakeUnitCube(f32 SideLen = 1.0f)
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
    
    mesh Result = MakeMesh(HelperMesh);
    
    return(Result);
}

mesh MakePlane()
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
    
    mesh Result = MakeMesh(HelperMesh);
    
    return(Result);
}

#include "asset_tool_assimp.cpp"