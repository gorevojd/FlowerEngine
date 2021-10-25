#ifndef FLOWER_ASSET_TYPES_H
#define FLOWER_ASSET_TYPES_H

#if 0
struct asset_model_offsets
{
    u32 OffsetMeshIDs;
    u32 OffsetMaterialIDs;
    u32 OffsetNodes;
    u32 OffsetNode_ToModel;
    u32 OffsetNode_ToParent;
    u32 OffsetNode_ParentIndex;
    u32 OffsetBone_InvBindPose;
    u32 OffsetBone_NodeIndex;
    u32 OffsetBone_SkinningMatrices;
};

struct asset_node_animation_offsets
{
    u32 OffsetPositionKeys;
    u32 OffsetRotationKeys;
    u32 OffsetScalingKeys;
    u32 OffsetPositionTimes;
    u32 OffsetRotationTimes;
    u32 OffsetScalingTimes;
};

struct asset_model
{
    // NOTE(Dima): Meshes
    u32* MeshIDs;
    u32* MaterialIDs;
    
    asset_model_shared Shared;
    model_offsets Offsets;
};

struct asset_animation
{
    char Name[64];
    
    asset_animation_shared Shared;
};
#endif


struct asset_image
{
    int Width;
    int Height;
    b32 FilteringIsNearest;
    
    u32 BlobDataOffset;
};

struct asset_skybox
{
    asset_id Left;
    asset_id Right;
    asset_id Front;
    asset_id Back;
    asset_id Up;
    asset_id Down;
};

struct asset_mesh
{
    int VertexCount;
    int IndexCount;
    
    u32 BlobOffsetP;
    u32 BlobOffsetUV;
    u32 BlobOffsetN;
    u32 BlobOffsetC;
    u32 BlobOffsetBoneWeights;
    u32 BlobOffsetBoneIDs;
    
    b32 IsSkinned;
    int MaterialIndexInModel;
};

struct asset_material
{
    char Name[64];
    
    u32 Type;
    
    asset_id TextureIDs[MAX_MATERIAL_TEXTURES];
};

struct asset_node_animation
{
    u32 BlobOffset_PosKeys;
    u32 BlobOffset_RotKeys;
    u32 BlobOffset_ScaKeys;
    
    u32 BlobOffset_PosTimes;
    u32 BlobOffset_RotTimes;
    u32 BlobOffset_ScaTimes;
    
    int NumPos;
    int NumRot;
    int NumSca;
    
    int NodeIndex;
};

struct asset_animation
{
    char Name[64];
    
    int NumNodeAnims;
    asset_id FirstNodeAnimID;
    
    f32 DurationTicks;
    f32 TicksPerSecond;
    
    u32 OutsideBehaviour;
};

#define ASSET_GUID_SIZE 64

struct asset
{
    char GUID[ASSET_GUID_SIZE];
    
    u32 Type;
    
    union
    {
        asset_image Image;
        asset_skybox Skybox;
        asset_mesh Mesh;
        asset_material Material;
        asset_animation Animation;
        asset_node_animation NodeAnimation;
    } Data;
    
    void* Ptr;
};

#endif //FLOWER_ASSET_TYPES_H
