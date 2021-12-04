#define GetAssetDataByID(storage, id, type) (type*)GetAssetDataByID_(storage, id)

INTERNAL_FUNCTION char* GenerateSpecialGUID(char* Buf, 
                                            int BufSize,
                                            const char* BaseGUID,
                                            const char* SpecialStr)
{
    const char* StringsToConcat[] = 
    {
        (char*)BaseGUID,
        "_",
        (char*)SpecialStr
    };
    
    ConcatBunchOfStrings(Buf, BufSize, 
                         StringsToConcat,
                         ArrLen(StringsToConcat));
    
    return (Buf);
}

INTERNAL_FUNCTION inline 
asset_id GetAssetID(asset_storage* Storage, 
                    const char* GUID)
{
    asset_id Result = 0;
    
    auto& Found = Storage->GuidToID.find(GUID);
    if (Found != Storage->GuidToID.end())
    {
        Result = Found->second;
    }
    
    return Result;
}

INTERNAL_FUNCTION inline 
asset* GetAssetByID(asset_storage* Storage, asset_id ID)
{
    asset* Asset = &Storage->Assets[ID];
    
    return Asset;
}

INTERNAL_FUNCTION inline
void* GetAssetDataByID_(asset_storage* Storage, asset_id ID)
{
    asset* Asset = GetAssetByID(Storage, ID);
    
    return Asset->DataPtr.Ptr;
}


INTERNAL_FUNCTION
void ProcessAssetHeader(asset* Asset, bool IsInit)
{
    asset_header* Header = &Asset->Header;
    
    Header->Ptr = Header->HeaderBytes;
    
    switch(Asset->Type)
    {
        case Asset_Model:
        {
            if (IsInit)
            {
                new (Asset->Header.Model) asset_header_model;
            }
            else
            {
                Asset->Header.Model->~asset_header_model();
            }
        }break;
    }
    
    if (!IsInit)
    {
        Header->Ptr = 0;
    }
    else if (IsInit && (Asset->Type != Asset_None))
    {
        StrictAssert(Asset->Header.Ptr);
    }
}

INTERNAL_FUNCTION 
asset_id AddAssetToStorage(asset_storage* Storage, 
                           const char* GUID,
                           u32 Type)
{
    asset_id NewAssetID = Storage->NumAssets++;
    asset* NewAsset = GetAssetByID(Storage, NewAssetID);
    
    NewAsset->IsSupplemental = false;
    NewAsset->Type = Type;
    CopyStringsSafe(NewAsset->GUID, 
                    ArrLen(NewAsset->GUID), 
                    GUID);
    
    
    // NOTE(Dima): Adding asset to (Guid to AssetID) mapping
    b32 FoundWithTheSameName = false;
    if (Storage->GuidToID.size() > 0)
    {
        auto Found = Storage->GuidToID.find(NewAsset->GUID);
        
        FoundWithTheSameName = Found != Storage->GuidToID.end();
    }
    
    /*
    // NOTE(Dima): When inserting asset to hashmap we have to make sure 
that asset with the same GUID has not been inserted into asset storage before.
*/
    Assert(!FoundWithTheSameName);
    
    Storage->GuidToID[NewAsset->GUID] = NewAssetID;
    
    // NOTE(Dima): Initializing header
    ProcessAssetHeader(NewAsset, true);
    
    return NewAssetID;
}

INTERNAL_FUNCTION
void FreeAsset(asset_storage* Storage, int AssetIndex)
{
    asset* Asset = &Storage->Assets[AssetIndex];
    
    // NOTE(Dima): Freeing DataPtr if it is set
    if (!Asset->IsSupplemental && Asset->DataPtr.Ptr)
    {
        free(Asset->DataPtr.Ptr);
    }
    Asset->DataPtr.Ptr = 0;
    
    // NOTE(Dima): Freeing asset header
    ProcessAssetHeader(Asset, false);
}

INTERNAL_FUNCTION 
void InitAssetStorage(asset_storage* Storage)
{
    Assert(Storage->NumAssets == 0);
    Assert(Storage->Initialized == false);
    
    new (Storage) asset_storage;
    
    Storage->Initialized = true;
    Storage->NumAssets = 0;
    Storage->GuidToID.reserve(ASSET_DEFAULT_COUNT_IN_TABLE);
    
    AddAssetToStorage(Storage, "NullAsset", Asset_None);
}

INTERNAL_FUNCTION 
void FreeAssetStorage(asset_storage* Storage)
{
    // NOTE(Dima): Freeing all assets sources & data pointers & headers in asset pack
    for (int AssetIndex = 1;
         AssetIndex < Storage->NumAssets;
         AssetIndex++)
    {
        // NOTE(Dima): Freeing asset header
        FreeAsset(Storage, AssetIndex);
    }
    
    Storage->~asset_storage();
}

