#ifndef FLOWER_OPENGL_H
#define FLOWER_OPENGL_H

#include "GL/glew.h"
#include "flower_render.h"
#include "flower_util_hashmap.h"

#include "flower_opengl_shader.h"

enum gbuffer_color_texture_types
{
    GBufferTex_Colors,
    GBufferTex_Normals,
    GBufferTex_Positions,
};

struct opengl_framebuffer_texture_params
{
    // NOTE(Dima): GL_R8, GL_RGBA8, GL_RGB32F, ...
    GLuint InternalFormat;
    
    // NOTE(Dima): Actual format GL_RGBA, GL_RGB, GL_BGRA....
    GLuint Format;
    
    // NOTE(Dima): Component type GL_UNSIGNED_BYTE, GL_FLOAT, GL_HALF_FLOAT, GL_UNSIGNED_SHORT_5_6_5
    GLuint Type;
    
    GLuint Filtering;
};

struct opengl_framebuffer
{
    u32 Framebuffer;
    
#define MAX_COLOR_ATTACHMENTS 8
    u32 ColorTextures[MAX_COLOR_ATTACHMENTS];
    u32 DepthTexture;
    
    iv2 Resolution;
    
    // NOTE(Dima): These ones are used only if allocated through Pool
    int IndexInPools;
    int IndexInUse;
};

struct opengl_array_object
{
    u32 VAO;
    u32 VBO;
    u32 EBO;
};

#if 0
enum render_target_type
{
    RenderTarget_GBuffer,
    
    RenderTarget_Composition,
    RenderTarget_Composition_Blur,
    
    RenderTarget_SSAO,
    RenderTarget_SSAO_Blur,
    
    RenderTarget_PostProc0,
    RenderTarget_PostProc1,
    
    RenderTarget_Bloom,
    RenderTarget_Bloom_1div2,
    RenderTarget_Bloom_1div4,
    RenderTarget_Bloom_1div8,
};
#endif

struct opengl_framebuffer_pool
{
    int Width;
    int Height;
    
#define MAX_FRAMEBUFFERS_IN_FRAMEBUFFER_POOL 128
    opengl_framebuffer* UseFramebuffers[MAX_FRAMEBUFFERS_IN_FRAMEBUFFER_POOL];
    opengl_framebuffer* FreeFramebuffers[MAX_FRAMEBUFFERS_IN_FRAMEBUFFER_POOL];
    
    int NumUse;
    int NumFree;
};

struct opengl_g_buffer
{
    u32 Framebuffer;
    
    int Width;
    int Height;
    
    u32 ColorSpec;
    u32 Normal;
    u32 Depth;
    u32 Positions;
};

struct opengl_loaded_shader
{
    opengl_shader* Shader;
    
    opengl_loaded_shader* NextInLoadedShaderList;
};

struct opengl_state
{
    memory_arena* Arena;
    opengl_loaded_shader* LoadedShadersList;
    
    opengl_shader* StdShader;
    opengl_shader* StdShadowShader;
    opengl_shader* UIRectShader;
    opengl_shader* VoxelShader;
    opengl_shader* VoxelShadowShader;
    opengl_shader* SSAOShader;
    opengl_shader* SSAOBlurShader;
    opengl_shader* LightingShader;
    opengl_shader* BoxBlurShader;
    opengl_shader* DilationShader;
    opengl_shader* PosterizeShader;
    opengl_shader* DepthOfFieldShader;
    opengl_shader* SkyShader;
    opengl_shader* RenderDepthShader;
    opengl_shader* VarianceShadowBlurShader;
    opengl_shader* RenderWaterShader;
    opengl_shader* CrtDisplayShader;
    
    opengl_framebuffer GBuffer;
    
    opengl_framebuffer SSAOBuffer;
    opengl_framebuffer SSAOBlurBuffer1;
    opengl_framebuffer SSAOBlurBuffer2;
    u32 SSAONoiseTex;
    
    // NOTE(Dima): Screen quad
    opengl_array_object ScreenQuad;
    
    // NOTE(Dima): Skybox cube
    opengl_array_object SkyboxCube;
    
    opengl_framebuffer ShadowMapsFramebuffer;
    int InitCascadesCount;
    
    // NOTE(Dima): Framebuffer pools
#define NUM_FRAMEBUFFER_POOLS (FramebufPoolType_Count * DownscaleRes_Count)
    opengl_framebuffer_pool FramebufferPools[NUM_FRAMEBUFFER_POOLS];
    
    int MaxCombinedTextureUnits;
};

#endif