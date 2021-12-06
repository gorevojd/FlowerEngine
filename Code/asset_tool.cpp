INTERNAL_FUNCTION void InitAssetLoadingContext(asset_tool* Tool)
{
    Tool->NumPacksInUse = 0;
}

INTERNAL_FUNCTION asset_pack* UseAssetPack(asset_tool* Tool, 
                                           char* PackName)
{
    Assert(Tool->NumPacksInUse < MAX_ASSET_PACKS);
    
    // NOTE(Dima): Getting asset pack
    int IndexInPacks = Tool->NumPacksInUse++;
    asset_pack* Pack = &Tool->Packs[IndexInPacks];
    Pack->IndexInPacks = IndexInPacks;
    
    // NOTE(Dima): Initializing pack
    InitAssetStorage(&Pack->AssetStorage);
    
    CopyStringsSafe(Pack->Name, ArrLen(Pack->Name), PackName);
    
    ClearString(Pack->FileName, ArrLen(Pack->FileName));
    AppendToString(Pack->FileName, ArrLen(Pack->FileName), PackName);
    AppendToString(Pack->FileName, ArrLen(Pack->FileName), ".pack");
    
    return Pack;
}

INTERNAL_FUNCTION void FreeAssetPack(asset_tool* Tool, asset_pack* Pack)
{
    Pack->IndexInPacks = -1;
    
    asset_storage* Storage = &Pack->AssetStorage;
    
    FreeAssetStorage(Storage);
    
    // NOTE(Dima): Deleting pack from packs array
    int CurPack = Pack->IndexInPacks;
    int LastPack = Tool->NumPacksInUse - 1;
    
    // TODO(Dima): Check if that does not miss something when copying by value
    asset_pack Temp = Tool->Packs[LastPack];
    Tool->Packs[LastPack] = Tool->Packs[CurPack];
    Tool->Packs[CurPack] = Temp;
}

INTERNAL_FUNCTION void 
WriteAssetPackToFile(asset_pack* Pack)
{
    
}