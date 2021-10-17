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
                                                    void* PixData)
{
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
    
    AllocateImageInternal(Result, Width, Height, ResultPixels);
    Result->FilteringIsClosest = FilteringIsClosest;
    Result->Handle = {};
    
    return(Result);
}

INTERNAL_FUNCTION inline v4 GetPixelColor(image* Image, int x, int y)
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
            
            
#if 1
            *Dst = PackRGBA(DstColor);
#else
            *Dst = PackRGBA(V4(0.0f, 1.0f, 0.0f, 1.0f));
#endif
            
            
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