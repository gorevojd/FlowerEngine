#ifndef FLOWER_ASSET_SOURCES_H
#define FLOWER_ASSET_SOURCES_H

#define MAX_ASSET_SOURCE_FILE_PATH 255

struct asset_source
{
    char FilePath[MAX_ASSET_SOURCE_FILE_PATH];
    
    loading_params Params;
};

#endif //FLOWER_ASSET_SOURCES_H
