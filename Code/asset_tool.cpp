#include "flower_asset_shared.cpp"
#include "asset_tool_adding.cpp"

INTERNAL_FUNCTION void InitAssetLoadingContext(asset_tool* Tool)
{
    for (int PackIndex = 0;
         PackIndex < MAX_ASSET_PACKS;
         PackIndex++)
    {
        asset_pack* Pack = &Tool->Packs[PackIndex];
        
        Pack->InUse = false;
        Pack->IndexInPacks = -1;
    }
}

INTERNAL_FUNCTION asset_pack* UseAssetPack(asset_tool* Tool, 
                                           char* PackName)
{
    Assert(Tool->NumPacksInUse < MAX_ASSET_PACKS);
    
    // NOTE(Dima): Getting asset pack
    int IndexInPacks = Tool->NumPacksInUse;
    asset_pack* Pack = &Tool->Packs[IndexInPacks];
    Pack->IndexInPacks = IndexInPacks;
    
    // NOTE(Dima): Initializing pack
    InitAssetStorage(&Pack->AssetStorage);
    
    ClearString(Pack->PackFileName, ArrLen(Pack->PackFileName));
    AppendToString(Pack->PackBlobName, ArrLen(Pack->PackBlobName), PackName);
    AppendToString(Pack->PackBlobName, ArrLen(Pack->PackBlobName), ".pack");
    
    return Pack;
}

INTERNAL_FUNCTION void FreeAssetPack(asset_tool* Tool, asset_pack* Pack)
{
    Pack->IndexInPacks = -1;
    
    asset_storage* Storage = &Pack->AssetStorage;
    
    FreeAssetStorage(Storage);
    
    // NOTE(Dima): Deleting pack from packs array
    int CurPack = Pack->IndexInPacks;
    int LastPack = Pack->NumPacksInUse - 1;
    
    // TODO(Dima): Check if that does not miss something when copying by value
    asset_pack Temp = Tool->Packs[LastPack];
    Tool->Packs[LastPack] = Tool->Packs[CurPack];
    Tool->Packs[CurPack] = Temp;
}


INTERNAL_FUNCTION void 
WriteAssetPackToFile(asset_pack* Pack)
{
    
}
