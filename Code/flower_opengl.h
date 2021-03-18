#ifndef TUTY_OPENGL_H
#define TUTY_OPENGL_H

#include "GL/glew.h"
#include "flower_render.h"

inline b32 OpenGLAttribIsValid(GLint Attrib)
{
    b32 Result = Attrib != -1;
    
    return(Result);
}

#define OPENGL_LOAD_ATTRIB(name) Result.##name##Attr = glGetAttribLocation(Result.ID, "In"#name)
#define OPENGL_LOAD_UNIFORM(name) Result.##name##Loc = glGetUniformLocation(Result.ID, #name)

struct opengl_shader
{
    GLuint ID;
    
    char Name[64];
    
    // NOTE(Dima): Attribs
    GLint PositionAttr;
    GLint TexCoordsAttr;
    GLint NormalAttr;
    GLint ColorAttr;
    GLint WeightsAttr;
    GLint BoneIDsAttr;
    GLint PosUVAttr;
    
    GLint InstanceModelTran1Attr;
    GLint InstanceModelTran2Attr;
    GLint InstanceModelTran3Attr;
    GLint InstanceModelTran4Attr;
    
    // NOTE(Dima): Uniforms
    GLint ViewProjectionLoc;
    GLint ProjectionLoc;
    GLint ViewLoc;
    GLint ModelLoc;
    GLint SkinningMatricesLoc;
    GLint SkinningMatricesCountLoc;
    GLint MeshIsSkinnedLoc;
    GLint UseInstancingLoc;
    
    GLint MultColorLoc;
    GLint TexDiffuseLoc;
    GLint HasDiffuseLoc;
    GLint MaterialMissingLoc;
    
    GLint TextureAtlasLoc;
    GLint ChunkAtLoc;
    GLint PerFaceDataLoc;
    
    // NOTE(Dima): Text rendering uniforms
    GLint IsImageLoc;
    GLint ImageLoc;
    GLint RectsColorsLoc;
    GLint RectsTypesLoc;
    GLint IsBatchLoc;
    
    void Use()
    {
        glUseProgram(this->ID);
    }
    
    // NOTE(Dima): Uniform Functions
    GLint GetLocation(const char* UniformName)
    {
        GLint Location = glGetUniformLocation(this->ID, UniformName);
        
        if(Location == -1)
        {
            printf("Shader: \"%s\": Uniform \"%s\" is either not loaded or doesn't exist at all!\n", 
                   Name,
                   UniformName);
        }
        
        return(Location);
    }
    
    void SetBool(const char* UniformName, b32 Value)
    {
        GLint Location = GetLocation(UniformName);
        
        glUniform1i(Location, Value);
    }
    
    void SetInt(const char* UniformName, int Value)
    {
        GLint Location = GetLocation(UniformName);
        
        glUniform1i(Location, Value);
    }
    
    void SetFloat(const char* UniformName, float Value)
    {
        GLint Location = GetLocation(UniformName);
        
        glUniform1f(Location, Value);
    }
    
    void SetVec2(const char* UniformName, v2 Value)
    {
        GLint Location = GetLocation(UniformName);
        
        glUniform2f(Location, Value.x, Value.y);
    }
    
    void SetVec2(const char* UniformName, float X, float Y)
    {
        GLint Location = GetLocation(UniformName);
        
        glUniform2f(Location, X, Y);
    }
    
    void SetVec3(const char* UniformName, v3 Value)
    {
        GLint Location = GetLocation(UniformName);
        
        glUniform3f(Location, Value.x, Value.y, Value.z);
    }
    
    void SetVec3(const char* UniformName, f32 x, f32 y, f32 z)
    {
        GLint Location = GetLocation(UniformName);
        
        glUniform3f(Location, x, y, z);
    }
    
    void SetVec3Array(const char* UniformName, v3* Array, int Count)
    {
        GLint Location = GetLocation(UniformName);
        
        glUniform3fv(Location, Count, (const GLfloat*)Array);
    }
    
    void SetVec4(const char* UniformName, v4 Value)
    {
        GLint Location = GetLocation(UniformName);
        
        glUniform4f(Location, Value.x, Value.y, Value.z, Value.w);
    }
    
    void SetVec4(const char* UniformName, f32 x, f32 y, f32 z, f32 w)
    {
        GLint Location = GetLocation(UniformName);
        
        glUniform4f(Location, x, y, z, w);
    }
    
    void SetMat4(const char* UniformName, float* Data)
    {
        GLint Location = GetLocation(UniformName);
        
        glUniformMatrix4fv(Location, 1, true, Data);
    }
    
    void SetMat4(const char* UniformName, const m44& Matrix)
    {
        GLint Location = GetLocation(UniformName);
        
        glUniformMatrix4fv(Location, 1, true, &Matrix.e[0]);
    }
    
    void SetMat4Array(const char* UniformName, m44* Array, int Count)
    {
        GLint Location = GetLocation(UniformName);
        
        glUniformMatrix4fv(Location, Count, true, (const GLfloat*)Array);
    }
    
    void SetTexture2D(const char* UniformName, GLuint Texture, int Slot)
    {
        GLint Location = GetLocation(UniformName);
        
        glActiveTexture(GL_TEXTURE0 + Slot);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glUniform1i(Location, Slot);
    }
    
    void SetTextureBuffer(const char* UniformName, GLuint Texture, int Slot)
    {
        GLint Location = GetLocation(UniformName);
        
        glActiveTexture(GL_TEXTURE0 + Slot);
        glBindTexture(GL_TEXTURE_BUFFER, Texture);
        glUniform1i(Location, Slot);
    }
};

struct opengl_framebuffer
{
    u32 Framebuffer;
    u32 Texture;
    
    int Width;
    int Height;
};

struct opengl_pp_framebuffer
{
    opengl_framebuffer FB;
    
    b32 IsInUseNow;
};

struct opengl_g_buffer
{
    u32 Framebuffer;
    
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

struct opengl_state
{
    opengl_shader StdShader;
    opengl_shader UIRectShader;
    opengl_shader VoxelShader;
    opengl_shader SSAOShader;
    opengl_shader SSAOBlurShader;
    opengl_shader LightingShader;
    opengl_shader BoxBlurShader;
    opengl_shader DilationShader;
    opengl_shader PosterizeShader;
    opengl_shader DepthOfFieldShader;
    opengl_shader SkyShader;
    
    opengl_g_buffer GBuffer;
    opengl_ssao SSAO;
    
    // NOTE(Dima): Screen quad
    u32 ScreenQuadVAO;
    u32 ScreenQuadVBO;
    
    // NOTE(Dima): Skybox cube
    u32 SkyboxCubeVAO;
    u32 SkyboxCubeVBO;
    
    opengl_pp_framebuffer PostProcFramebufferPool[4];
};

#endif