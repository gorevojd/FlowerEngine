inline void PushSky(asset_storage* Storage, b32 Immediate, asset_id ID)
{
    cubemap* Sky = GetAssetDataByID(Storage, ID, cubemap);
    
    if (Sky)
    {
        PushSky(Sky);
    }
    else
    {
        StartAssetLoading(Storage, ID, Immediate);
    }
}

inline void PushMesh(asset_storage* Storage, 
                     b32 Immediate,
                     asset_id MeshID,
                     asset_id MaterialID,
                     const m44& ModelToWorld = IdentityMatrix4(),
                     v3 C = V3(1.0f, 1.0f, 1.0f),
                     m44* SkinningMatrices = 0,
                     int SkinningMatricesCount = 0)
{
    mesh* Mesh = GetAssetDataByID(Storage, MeshID, mesh);
    material* Material = 0;
    
    if (Mesh)
    {
        Material = GetAssetDataByID(Storage, MaterialID, material);
        if (Material)
        {
            
        }
        else
        {
            StartAssetLoading(Storage, MaterialID, Immediate);
            
            // TODO(Dima): Is mode is not immediate -> add ability to load temp (missing) material
        }
        
        PushMesh(Mesh, 
                 Material,
                 ModelToWorld,
                 C,
                 SkinningMatrices,
                 SkinningMatricesCount);
    }
    else
    {
        StartAssetLoading(Storage, MeshID, Immediate);
        
        // TODO(Dima): Is mode is not immediate -> add ability to load temp (missing) mesh
    }
    
#if 0
    PushMesh(Mesh, 
             Material,
             ModelToWorld,
             C,
             SkinningMatrices,
             SkinningMatricesCount);
#endif
}

