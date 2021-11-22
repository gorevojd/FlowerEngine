INTERNAL_FUNCTION helper_mesh CombineHelperMeshes(const helper_mesh* A, const helper_mesh* B)
{
    helper_mesh Result = {};
    
    Result.IsSkinned = A->IsSkinned;
    Result.Name = A->Name;
    
    // NOTE(Dima): Copy positions
    std::copy(A->Vertices.begin(), A->Vertices.end(), std::back_inserter(Result.Vertices));
    std::copy(B->Vertices.begin(), B->Vertices.end(), std::back_inserter(Result.Vertices));
    
    // NOTE(Dima):  copy tex coords
    std::copy(A->TexCoords.begin(), A->TexCoords.end(), std::back_inserter(Result.TexCoords));
    std::copy(B->TexCoords.begin(), B->TexCoords.end(), std::back_inserter(Result.TexCoords));
    
    // NOTE(Dima): Copy colors
    std::copy(A->Colors.begin(), A->Colors.end(), std::back_inserter(Result.Colors));
    std::copy(B->Colors.begin(), B->Colors.end(), std::back_inserter(Result.Colors));
    
    // NOTE(Dima): Copy normals
    std::copy(A->Normals.begin(), A->Normals.end(), std::back_inserter(Result.Normals));
    std::copy(B->Normals.begin(), B->Normals.end(), std::back_inserter(Result.Normals));
    
    // NOTE(Dima): Copy bone weights
    std::copy(A->BoneWeights.begin(), A->BoneWeights.end(), std::back_inserter(Result.BoneWeights));
    std::copy(B->BoneWeights.begin(), B->BoneWeights.end(), std::back_inserter(Result.BoneWeights));
    
    // NOTE(Dima): Bone indices
    std::copy(A->BoneIndices.begin(), A->BoneIndices.end(), std::back_inserter(Result.BoneIndices));
    std::copy(B->BoneIndices.begin(), B->BoneIndices.end(), std::back_inserter(Result.BoneIndices));
    
    // NOTE(Dima): Copy ids and recalculate ids from second array
    std::copy(A->Indices.begin(), A->Indices.end(), std::back_inserter(Result.Indices));
    for(int IndexIndex = 0;
        IndexIndex < B->Indices.size();
        IndexIndex++)
    {
        Result.Indices.push_back(B->Indices[IndexIndex] + A->Vertices.size());
    }
    
    return(Result);
}

INTERNAL_FUNCTION inline void AllocateImageInternal(image* Image,
                                                    u32 Width, u32 Height, 
                                                    void* PixData,
                                                    b32 FilteringIsClosest = false)
{
    Image->Handle = {};
    
    Image->FilteringIsClosest = FilteringIsClosest;
    Image->Width = Width;
	Image->Height = Height;
    
	Image->WidthOverHeight = (float)Width / (float)Height;
    
	Image->Pixels = PixData;
}

INTERNAL_FUNCTION image* AllocateImage(u32 Width, u32 Height, 
                                       b32 FilteringIsClosest = false)
{
    mi DataSize = Width * Height * 4;
    mi OffsetToPixelsData = Align(sizeof(image), 64);
    
    void* ResultData = malloc(OffsetToPixelsData + DataSize);
    
    image* Result = (image*)ResultData;
    void* ResultPixels = (u8*)ResultData + OffsetToPixelsData;
    
    memset(ResultPixels, 0, DataSize);
    
    AllocateImageInternal(Result, Width, Height, ResultPixels, FilteringIsClosest);
    
    return(Result);
}

INTERNAL_FUNCTION material* AllocateMaterial(u32 MaterialType = Material_SpecularDiffuse)
{
    material* Material = (material*)malloc(sizeof(material));
    
    Material->Type = MaterialType;
    
    return Material;
}

INTERNAL_FUNCTION 
void FillImageWithColor(image* Image, v4 ClearColor, b32 PremultiplyColorAlpha = true)
{
    v4 Color = ClearColor;
    if (PremultiplyColorAlpha)
    {
        Color = PremultiplyAlpha(Color);
    }
    
    u32 Packed = PackRGBA(Color);
    
    for (int y = 0; y < Image->Height; y++)
    {
        for (int x = 0; x < Image->Width; x++)
        {
            u32* Pixel = (u32*)Image->Pixels + y * Image->Width + x;
            
            *Pixel = Packed;
        }
    }
}

INTERNAL_FUNCTION
void ClearImage(image* Image)
{
    FillImageWithColor(Image, V4(0.0f));
}

INTERNAL_FUNCTION inline 
v4 GetPixelColor(image* Image, int x, int y)
{
    Assert(x < Image->Width);
    Assert(y < Image->Height);
    
    u32* At = (u32*)Image->Pixels + y * Image->Width + x;
    
    v4 Result = UnpackRGBA(*At);
    
    return(Result);
}

INTERNAL_FUNCTION void CopyImage(image* Dst,
                                 image* Src)
{
    Assert(Dst->Width == Src->Width);
    Assert(Dst->Height == Src->Height);
    
    memcpy(Dst->Pixels, Src->Pixels, 
           Dst->Width * Dst->Height * 4);
    
    Dst->Handle = {};
}

INTERNAL_FUNCTION void ConvertGrayscaleToRGBA(u32* RGBA, u8* Grayscale,
                                              int Width, 
                                              int Height)
{
    for (int y = 0; y < Height; y++)
    {
        for (int x = 0; x < Width; x++)
        {
            u8* Src = Grayscale + y * Width + x;
            u32* Dst = RGBA + y * Width + x;
            
            f32 SrcValue = (f32)(*Src) * F_ONE_OVER_255;
            v4 DstColor = V4(SrcValue);
            
            *Dst = PackRGBA(DstColor);
        }
    }
}

INTERNAL_FUNCTION void ConvertGrayscaleToRGBA(image* Image,
                                              u8* Grayscale)
{
    ConvertGrayscaleToRGBA((u32*)Image->Pixels,
                           Grayscale,
                           Image->Width,
                           Image->Height);
}

INTERNAL_FUNCTION inline void InvalidateImage(image* Image)
{
    Image->Handle.Invalidated = true;
}

INTERNAL_FUNCTION u32 UTF8_Sequence_Size_For_UCS4(u32 u)
{
    // Returns number of bytes required to encode 'u'
    static const u32 CharBounds[] = { 
        0x0000007F, 
        0x000007FF, 
        0x0000FFFF, 
        0x001FFFFF, 
        0x03FFFFFF, 
        0x7FFFFFFF, 
        0xFFFFFFFF };
    
    u32 bi = 0;
    while(CharBounds[bi] < u ){
        ++bi;
    }
    return bi+1;
}

// NOTE(Dima): Thanx Alex Podverbny (BLK Dragon) 4 the code
INTERNAL_FUNCTION u32 UTF16_To_UTF8(u16* UTF16String, 
                                    u8* To, u32 ToLen, 
                                    u32* OutToSize)
{
    u8*         s       = To;
    u8*         s_end   = To + ToLen;
    u16*  w       = UTF16String;
    u32        len     = 0;
    while(*w)
    {
        u32    ch = *w ;
        u32    sz = UTF8_Sequence_Size_For_UCS4( ch );
        if( s + sz >= s_end )
            break;
        if(sz == 1)
        {
            // just one byte, no header
            *s = (u8)(ch);
            ++s;
        }
        else
        {
            // write the bits 6 bits at a time, 
            // except for the first one, which can be less than 6 bits
            u32 shift = (sz-1) * 6;
            *s = uint8_t(((ch >> shift) & 0x3F) | (0xFF << (8 - sz)));
            shift -= 6;
            ++s;
            for(u32 i=1; i!=sz; ++i,shift-=6 )
            {
                *s = u8(((ch >> shift) & 0x3F) | 0x80);
                ++s;
            }
        }
        ++len;
        ++w;
    }
    
    *s = 0x00;
    if(OutToSize){
        *OutToSize = (s - To);
    }
    
    return len;
}