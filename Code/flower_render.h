#ifndef FLOWER_RENDER_H
#define FLOWER_RENDER_H

#include "flower_lighting.h"
#include "flower_postprocess.h"

#define RENDER_DEFAULT_2D_LINE_THICKNESS 4.0f
#define RENDER_DEFAULT_2D_LINE_DASH_LENGTH 20.0f
#define RENDER_DEFAULT_2D_LINE_DASH_SPACING 8.0f
#define RENDER_MAX_BONES 256

enum class culling_mode : u32
{
    AABB,
    Sphere,
};

struct culling_info
{
    culling_mode Mode;
    
    union
    {
        struct 
        {
            v3 Sphere_Center;
            f32 Sphere_Radius;
        };
        
        struct 
        {
            v3 AABB_Center;
            v3 AABB_Radius;
        };
    };
};

inline culling_info CullingInfoSphere(v3 Center, f32 Rad)
{
    culling_info Result = {};
    
    Result.Mode = culling_mode::Sphere;
    Result.Sphere_Center = Center;
    Result.Sphere_Radius = Rad;
    
    return(Result);
}

inline culling_info CullingInfoAABB(v3 Center, v3 HalfDim)
{
    culling_info Result = {};
    
    Result.Mode = culling_mode::AABB;
    Result.AABB_Center = Center;
    Result.AABB_Radius = HalfDim;
    
    return(Result);
}

enum render_command_type
{
    RenderCommand_Clear,
    RenderCommand_Mesh,
    RenderCommand_InstancedMesh,
    RenderCommand_VoxelChunkMesh,
};

#define RENDER_COMMAND_STRUCT(type) render_command_##type

#pragma pack(push, 8)
enum render_command_clear_flags
{
    RenderClear_Color = (1 << 0),
    RenderClear_Depth = (1 << 1),
    RenderClear_Stencil = (1 << 2),
};

struct render_command_clear
{
    b32 Set;
    
    v3 C;
    u32 Flags;
};

struct render_command_image
{
    image* Image;
    
    v2 P;
    v2 Dim;
    v4 C;
    
    render_command_image* Next;
    render_command_image* Prev;
};

struct render_command_mesh
{
    mesh* Mesh;
    material* Material;
    m44* SkinningMatrices;
    v3 C;
    int SkinningMatricesCount;
    
    m44 ModelToWorld;
};

struct render_command_instanced_mesh
{
    mesh* Mesh;
    material* Material;
    v3 C;
    
    m44* InstanceSkinningMatrices;
    int NumSkinningMatricesPerInstance;
    
    m44* InstanceMatrices;
    int InstanceCount;
    int MaxInstanceCount;
};

struct render_voxel_mesh_layout
{
    int VL_ShiftX;
    int VL_ShiftY;
    int VL_ShiftZ;
    
    int VL_MaskX;
    int VL_MaskY;
    int VL_MaskZ;
};

struct render_command_voxel_mesh
{
    voxel_mesh* Mesh;
    
    v3 ChunkAt;
    
    render_voxel_mesh_layout Layout;
    culling_info CullingInfo;
};

struct render_mesh_instance
{
    render_mesh_instance* NextInHash;
    
    mesh* Mesh;
    render_command_instanced_mesh* Command;
};

struct render_command_header
{
    u32 CommandType;
    void* CommandData;
};
#pragma pack(pop)

struct rect_vertex
{
    v2 P;
    v2 UV;
};

enum rect_type
{
    Rect_Textured = 0,
    Rect_Solid = 1,
};

struct rect_buffer
{
#define MAX_RECTS_COUNT 30000
    rect_vertex Vertices[MAX_RECTS_COUNT * 4];
    u32 Indices[MAX_RECTS_COUNT * 6];
    u32 Colors[MAX_RECTS_COUNT];
    u8 Types[MAX_RECTS_COUNT];
    
    int RectCount;
};

struct window_dimensions
{
    int InitWidth;
    int InitHeight;
    
    int Width;
    int Height;
};

struct render_api_dealloc_entry
{
    renderer_handle* Handle;
    
    render_api_dealloc_entry* Next;
    render_api_dealloc_entry* Prev;
};

struct render_pass
{
    m44 View;
    m44 Projection;
    m44 ViewProjection;
    
    v3 CameraLeft;
    v3 CameraUp;
    v3 CameraFront;
    v3 CameraP;
    
    f32 Far;
    f32 Near;
    f32 FOVDegrees;
    f32 Width;
    f32 Height;
    f32 AspectRatio;
    
    v4 FrustumPlanes[6];
    
    b32 IsShadowPass;
    
    b32 ClippingPlaneIsSet;
    v4 ClippingPlane;
};

struct render_water_params
{
    f32 Height;
    
    v4 Color;
};

struct render_water
{
    render_pass* ReflectionPass;
    render_pass* RefractionPass;
    
    v4 PlaneEquation;
    
    render_water_params Params;
};

inline b32 IsSphereCulledByFrustum(v4 Planes[6], v3 Center, f32 Rad)
{
    b32 Result = false;
    
    for(int i = 0; i < 6; i++)
    {
        f32 PlaneTest = PlanePointTest(Planes[i], Center);
        if(PlaneTest + Rad < 0.0f)
        {
            // NOTE(Dima): It means that culling happened on one of sides and object will not be visible
            Result = true;
            break;
        }
    }
    
    return(Result);
}

inline b32 IsFrustumCulled(render_pass* Pass, culling_info* Culling, b32 CullingEnabled)
{
    b32 Result = false;
    
    if(CullingEnabled)
    {
        switch(Culling->Mode)
        {
            case culling_mode::Sphere:
            {
                Result = IsSphereCulledByFrustum(Pass->FrustumPlanes,
                                                 Culling->Sphere_Center,
                                                 Culling->Sphere_Radius); 
            }break;
            
            case culling_mode::AABB:
            {
                v3 C = Culling->AABB_Center;
                v3 HalfDim = Culling->AABB_Radius;
                
                Result = IsSphereCulledByFrustum(Pass->FrustumPlanes,
                                                 C,
                                                 Length(HalfDim));
            }break;
        }
    }
    
    return(Result);
}

enum render_sky_type
{
    RenderSky_SolidColor,
    RenderSky_Gradient,
    RenderSky_Skybox,
};

struct material_command_entry
{
    render_command_header Header;
    
    material_command_entry* Next;
};

struct material_commands
{
    u32 MaterialID;
    
    material_command_entry* FirstEntry;
};

struct sorted_by_material_commands
{
    material_commands MaterialCommands[128];
    int Count;
};

struct render_commands
{
    memory_arena CommandsBuffer;
    memory_arena* Arena;
    
    m44 ScreenOrthoProjection;
    rect_buffer Rects2D;
    
    void* StateOfGraphicsAPI;
    
    // TODO(Dima): Make those dynamic
#define MAX_RENDER_COMMANDS_COUNT 200000
    render_command_header CommandHeaders[MAX_RENDER_COMMANDS_COUNT];
    int CommandCount;
    
    sorted_by_material_commands SortedMaterialsOpaque;
    sorted_by_material_commands SortedMaterialsTransparent;
    material_command_entry* FirstFreeCommandEntry;
    
    render_pass RenderPasses[128];
    int RenderPassCount;
    
    render_command_image ImageUse;
    render_command_image ImageFree;
    
    window_dimensions WindowDimensions;
    image* FontAtlas;
    image* VoxelAtlas;
    
    render_water Water;
    b32 WaterIsSet;
    
    // NOTE(Dima): Sky stuff
    cubemap* Sky;
    int SkyType;
    int DefaultSkyType;
    v3 DefaultSkyColor;
    v3 SkyColor;
    f32 Time;
    
    // NOTE(Dima): Clear command stuff
    render_command_clear ClearCommand;
    
    u8* VoxelTempData;
    mi PrevVoxelDataSize;
    
    u32* VoxelVerts;
    u32* VoxelFaces;
    u16* FaceToChunk;
    v3* VoxelChunksP;
    
    b32 CullingEnabled;
    
    // NOTE(Dima): Instance table
#define RENDER_INSTANCE_TABLE_SIZE 256
    render_mesh_instance* InstanceTable[RENDER_INSTANCE_TABLE_SIZE];
    
    lighting Lighting;
    postprocessing PostProcessing;
    
    // NOTE(Dima): Deallocate entries
    ticket_mutex DeallocEntriesMutex;
    render_api_dealloc_entry UseDealloc;
    render_api_dealloc_entry FreeDealloc;
};

inline void* GetRenderCommand_(render_commands* Commands, int CommandIndex)
{
    render_command_header* Header = &Commands->CommandHeaders[CommandIndex];
    
    void* Result = Header->CommandData;
    
    return(Result);
}
#define GetRenderCommand(commands, index, struct_type) (struct_type*)GetRenderCommand_(commands, index)

#endif //FLOWER_RENDER_H
