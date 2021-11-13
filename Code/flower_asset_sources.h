#ifndef FLOWER_ASSET_SOURCES_H
#define FLOWER_ASSET_SOURCES_H

struct asset_source_image
{
    char* FilePath;
    
    loading_params Params;
};

struct asset_source_cubemap
{
    asset_id Image_Left;
    asset_id Image_Right;
    asset_id Image_Front;
    asset_id Image_Back;
    asset_id Image_Up;
    asset_id Image_Down;
};

union asset_source
{
    asset_source_image* Image;
    asset_source_cubemap* Cubemap;
};

#endif //FLOWER_ASSET_SOURCES_H
