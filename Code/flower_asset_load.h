#ifndef FLOWER_ASSET_LOAD_H
#define FLOWER_ASSET_LOAD_H

struct loading_params
{
    u32 AssetType;
    
    union
    {
        struct 
        {
            b32 FlipVertically;
            b32 FilteringIsClosest;
            v2 Align;
        }Image;
        
        struct 
        {
            v4 ShadowColor;
            v4 OutlineColor;
            int ShadowOffset;
            int AtlasWidth;
            u32 SizesFlags;
            u32 StyleFlags;
        }Font;
        
        struct 
        {
            f32 DefaultScale;
            b32 FixInvalidRotation;
        }Model;
    };
};

inline loading_params LoadingParams(u32 AssetType)
{
    loading_params Result = {};
    
    return(Result);
}

inline loading_params LoadingParams_Internal(u32 AssetType)
{
    loading_params Result = {};
    
    Result.AssetType = AssetType;
    
    return Result;
}

inline loading_params LoadingParams_Image()
{
    loading_params Result = LoadingParams_Internal(Asset_Image);
    
    // NOTE(Dima): Image
    Result.Image.FlipVertically = true;
    Result.Image.FilteringIsClosest = false;
    Result.Image.Align = V2(0.0f, 0.0f);
    
    return Result;
}

inline loading_params LoadingParams_Font()
{
    loading_params Result = LoadingParams_Internal(Asset_Font);
    
    // NOTE(Dima): Font
    Result.Font.ShadowOffset = 1;
    Result.Font.ShadowColor = ColorBlack();
    Result.Font.OutlineColor = ColorBlack();
    Result.Font.AtlasWidth = 2048;
    Result.Font.SizesFlags = 0;
    //Result.Font.StyleFlags = GlyphStyle_Original | GlyphStyle_Shadow | GlyphStyle_Outline;
    Result.Font.StyleFlags = GlyphStyle_Original;
    
    return Result;
}

inline loading_params LoadingParams_Model()
{
    loading_params Result = LoadingParams_Internal(Asset_Model);
    
    // NOTE(Dima): Model
    Result.Model.FixInvalidRotation = false;
    Result.Model.DefaultScale = 1.0f;
    
    return Result;
}

#endif //FLOWER_ASSET_LOAD_H
