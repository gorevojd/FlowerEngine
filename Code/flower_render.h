#ifndef FLOWER_RENDER_H
#define FLOWER_RENDER_H

enum framebuffer_pool_type
{
    FramebufPoolType_Color,
    FramebufPoolType_HDR,
    FramebufPoolType_SSAO,
    
    FramebufPoolType_Count,
};

enum rt_downscale_res
{
    DownscaleRes_1,
    DownscaleRes_1div2,
    DownscaleRes_1div4,
    //DownscaleRes_1div8,
    
    DownscaleRes_Count,
};

struct framebuffer_in_pool_params
{
    u32 Type;
    u32 Resolution;
};

enum type_color_output
{
    ColorOutput_MainColor,
    ColorOutput_SSAO,
    ColorOutput_LinearDepth,
    ColorOutput_Normals,
    
    ColorOutput_Count,
};

#include "flower_lighting.h"
#include "flower_postprocess.h"

#define RENDER_DEFAULT_2D_LINE_THICKNESS 4.0f
#define RENDER_DEFAULT_2D_LINE_DASH_LENGTH 20.0f
#define RENDER_DEFAULT_2D_LINE_DASH_SPACING 8.0f
#define RENDER_MAX_BONES 256

// NOTE(Dima): Renderer functions
#define PLATFORM_RENDERER_BEGIN_FRAME(name) void name(struct render_commands* Commands)
typedef PLATFORM_RENDERER_BEGIN_FRAME(platform_renderer_begin_frame);

#define PLATFORM_RENDERER_RENDER(name) void name(struct render_commands* Commands)
typedef PLATFORM_RENDERER_RENDER(platform_renderer_render);

#define PLATFORM_RENDERER_PRESENT(name) void name(struct render_commands* Commands)
typedef PLATFORM_RENDERER_PRESENT(platform_renderer_present);

struct renderer_api
{
    platform_renderer_begin_frame* BeginFrame;
    platform_renderer_render* Render;
    platform_renderer_present* Present;
};

extern renderer_api RenderAPI;

#if 0
enum texture_components
{
    TextureComponents_RGBA,
    TextureComponents_RGB,
    TextureComponents_RG,
    TextureComponents_R,
};

enum texture_component_format
{
    TextureComponentFormat_Float32,
    TextureComponentFormat_Float16,
    TextureComponentFormat_UnsignedByte,
};

struct texture_params
{
    int Width;
    int Height;
    u32 TextureComponents;
    u32 ComponentFormat;
    b32 IsSRGB;
};
#endif

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
    RenderCommand_Image,
    RenderCommand_RectBuffer,
    RenderCommand_Mesh,
    RenderCommand_RectBatch,
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
    b32 DisableDepthTest;
};

struct render_command_rect_buffer
{
    struct batch_rect_buffer* RectBuffer;
    
    b32 DisableDepthTest;
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

#define RECT_VERTEX_UNTEXTURED 255

struct batch_rect_buffer
{
    rect_vertex* Vertices;
    u32* Indices;
    u32* Colors;
    
    // If Texture index is 255 - it means that rect should be solid
    u8* TextureIndices;
    
    m44 ViewProjection;
    
    image* TextureAtlases[14];
    int TextureCount;
    
    int RectCount;
    int MaxRectCount;
};

struct window_dimensions
{
    iv2 Init;
    iv2 Current;
    
#if 0    
    int InitWidth;
    int InitHeight;
    
    int Width;
    int Height;
#endif
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

struct render_commands
{
    memory_arena CommandsBuffer;
    memory_arena* Arena;
    
    b32 IsDeferredRenderer;
    
    u32 ActiveFrameUniqueFonts[14];
    int ActiveFrameUniqueFontsCount;
    
    batch_rect_buffer* DEBUG_Rects2D_Window;
    batch_rect_buffer* DEBUG_Rects2D_Unit;
    
    void* StateOfGraphicsAPI;
    
    // TODO(Dima): Make those dynamic
#define MAX_RENDER_COMMANDS_COUNT 200000
    render_command_header CommandHeaders[MAX_RENDER_COMMANDS_COUNT];
    int CommandCount;
    
    render_pass RenderPasses[128];
    int RenderPassCount;
    
    window_dimensions WindowDimensions;
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
    post_processing PostProcessing;
    
    // NOTE(Dima): Deallocate entries
    ticket_mutex DeallocEntriesMutex;
    render_api_dealloc_entry UseDealloc;
    render_api_dealloc_entry FreeDealloc;
    
    int TypeColorOutput;
};

inline void* GetRenderCommand_(render_commands* Commands, int CommandIndex)
{
    render_command_header* Header = &Commands->CommandHeaders[CommandIndex];
    
    void* Result = Header->CommandData;
    
    return(Result);
}
#define GetRenderCommand(commands, index, struct_type) (struct_type*)GetRenderCommand_(commands, index)

#endif //FLOWER_RENDER_H
