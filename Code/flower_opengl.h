#ifndef TUTY_OPENGL_H
#define TUTY_OPENGL_H

#include "GL/glew.h"
#include "flower_render.h"
#include "flower_util_hashmap.h"

#include "flower_opengl_shader.h"

struct opengl_framebuffer
{
    u32 Framebuffer;
    u32 Texture;
    u32 DepthTexture;
    
    int Width;
    int Height;
    
    int ResolutionIndex;
    int IndexInUse;
};

struct opengl_array_object
{
    u32 VAO;
    u32 VBO;
    u32 EBO;
};

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

struct opengl_ssao
{
    // NOTE(Dima): SSAO
    u32 Framebuffer;
    u32 FramebufferTexture;
    
    u32 BlurFramebuffer;
    u32 BlurFramebufferTexture;
    
    u32 NoiseTex;
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
    
    opengl_g_buffer GBuffer;
    opengl_ssao SSAO;
    u32 PoissonSamplesRotationTex;
    
    // NOTE(Dima): Screen quad
    opengl_array_object ScreenQuad;
    
    // NOTE(Dima): Skybox cube
    opengl_array_object SkyboxCube;
    
    opengl_framebuffer ShadowMap;
    int InitCascadesCount;
    
    // NOTE(Dima): Framebuffer pools
    opengl_framebuffer_pool FramebufferPoolResolutions[PostProcResolution_Count];
    
    int MaxCombinedTextureUnits;
};

#endif