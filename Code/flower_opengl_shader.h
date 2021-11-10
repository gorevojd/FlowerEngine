/* date = October 12th 2021 7:04 pm */

#ifndef FLOWER_OPENGL_SHADER_H
#define FLOWER_OPENGL_SHADER_H

inline b32 OpenGLAttribIsValid(GLint Attrib)
{
    b32 Result = Attrib != -1;
    
    return(Result);
}

struct uniform_name_entry
{
    const char* Name;
    u32 NameHash;
    GLint Location;
};

enum opengl_shader_type
{
    OpenGL_Shader_Default,
    OpenGL_Shader_Compute,
};

struct opengl_shader
{
    memory_arena* Arena;
    u32 ID;
    u32 Type;
    
    const char* PathV;
    const char* PathF;
    const char* PathG;
    
    char Name[64];
    u32 NameHash;
    
    // NOTE(Dima): This map will cache all locations that were queried
    hashmap<uniform_name_entry, 256> Name2Loc;
    hashmap<uniform_name_entry, 256> Name2Attrib;
    
    void Use()
    {
        glUseProgram(this->ID);
    }
    
    static int GetCurrentProgram()
    {
        GLint CurrProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &CurrProgram);
        
        return CurrProgram;
    }
    
    GLint GetAttribLoc(const char* AttribName)
    {
        GLint Location;
        
        u32 Hash = StringHashFNV((char*)AttribName);
        uniform_name_entry* Found = Name2Attrib.find(Hash);
        
        if (Found)
        {
            Location = Found->Location;
        }
        else
        {
            Location = glGetAttribLocation(this->ID, AttribName);
            
            if(Location == -1)
            {
                printf("Shader: \"%s\": Attribute \"%s\" is either not loaded, does not exit, or not used!\n", 
                       Name,
                       AttribName);
            }
            
            uniform_name_entry New = {};
            
            New.Name = AttribName;
            New.NameHash = Hash;
            New.Location = Location;
            
            Name2Attrib.insert(Hash, New);
        }
        
        return(Location);
    }
    
    // NOTE(Dima): Uniform Functions
    GLint GetLoc(const char* UniformName)
    {
        GLint Location;
        
        u32 Hash = StringHashFNV((char*)UniformName);
        uniform_name_entry* Found = Name2Loc.find(Hash);
        
        if (Found)
        {
            Location = Found->Location;
        }
        else
        {
            Location = glGetUniformLocation(this->ID, UniformName);
            
            if(Location == -1)
            {
                printf("Shader: \"%s\": Uniform \"%s\" is either not loaded, does not exit, or not used!\n", 
                       Name,
                       UniformName);
            }
            
            uniform_name_entry New = {};
            
            New.Name = UniformName;
            New.NameHash = Hash;
            New.Location = Location;
            
            Name2Loc.insert(Hash, New);
        }
        
        return(Location);
    }
    
    void SetBool(const char* UniformName, b32 Value)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniform1i(Location, Value);
    }
    
    void SetInt(const char* UniformName, int Value)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniform1i(Location, Value);
    }
    
    void SetFloat(const char* UniformName, float Value)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniform1f(Location, Value);
    }
    
    void SetFloatArray(const char* UniformName, float* Values, int Count)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniform1fv(Location, Count, (const GLfloat*)Values);
    }
    
    void SetVec2(const char* UniformName, v2 Value)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniform2f(Location, Value.x, Value.y);
    }
    
    void SetVec2(const char* UniformName, float X, float Y)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniform2f(Location, X, Y);
    }
    
    void SetVec2Array(const char* UniformName, v2* Array, int Count)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniform2fv(Location, Count, (const GLfloat*)Array);
    }
    
    void SetVec3(const char* UniformName, v3 Value)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniform3f(Location, Value.x, Value.y, Value.z);
    }
    
    void SetVec3(const char* UniformName, f32 x, f32 y, f32 z)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniform3f(Location, x, y, z);
    }
    
    void SetIVec3(const char* UniformName, int x, int y, int z)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniform3i(Location, x, y, z);
    }
    
    void SetUIVec3(const char* UniformName, u32 x, u32 y, u32 z)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniform3ui(Location, x, y, z);
    }
    
    void SetVec3Array(const char* UniformName, v3* Array, int Count)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniform3fv(Location, Count, (const GLfloat*)Array);
    }
    
    void SetVec4(const char* UniformName, v4 Value)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniform4f(Location, Value.x, Value.y, Value.z, Value.w);
    }
    
    void SetVec4(const char* UniformName, f32 x, f32 y, f32 z, f32 w)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniform4f(Location, x, y, z, w);
    }
    
    void SetMat4(const char* UniformName, float* Data)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniformMatrix4fv(Location, 1, true, Data);
    }
    
    void SetMat4(const char* UniformName, const m44& Matrix)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniformMatrix4fv(Location, 1, true, &Matrix.e[0]);
    }
    
    void SetMat4Array(const char* UniformName, m44* Array, int Count)
    {
        GLint Location = GetLoc(UniformName);
        
        glUniformMatrix4fv(Location, Count, true, (const GLfloat*)Array);
    }
    
    void SetTexture2D(const char* UniformName, GLuint Texture, int Slot)
    {
        GLint Location = GetLoc(UniformName);
        
        glActiveTexture(GL_TEXTURE0 + Slot);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glUniform1i(Location, Slot);
    }
    
    
    void SetTexture2DArray(const char* UniformName, GLuint Texture, int Slot)
    {
        GLint Location = GetLoc(UniformName);
        
        glActiveTexture(GL_TEXTURE0 + Slot);
        glBindTexture(GL_TEXTURE_2D_ARRAY, Texture);
        glUniform1i(Location, Slot);
    }
    
    void SetTextureBuffer(const char* UniformName, GLuint Texture, int Slot)
    {
        GLint Location = GetLoc(UniformName);
        
        glActiveTexture(GL_TEXTURE0 + Slot);
        glBindTexture(GL_TEXTURE_BUFFER, Texture);
        glUniform1i(Location, Slot);
    }
};

#endif //FLOWER_OPENGL_SHADER_H
