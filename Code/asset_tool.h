#ifndef ASSET_TOOL_H
#define ASSET_TOOL_H

struct asset_pack
{
    asset_storage AssetStorage;
    
    int IndexInPacks;
    
    char Name[256];
    char FileName[256];
};

struct asset_tool
{
#define MAX_ASSET_PACKS 64
    asset_pack Packs[MAX_ASSET_PACKS];
    int NumPacksInUse;
};

#endif //ASSET_TOOL_H
