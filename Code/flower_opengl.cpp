#include "flower_postprocess_shared.cpp"

#define OPENGL_INIT_FRAMEBUFFER_FLOW(name) void name(opengl_framebuffer* Buffer, int Width, int Height)
typedef OPENGL_INIT_FRAMEBUFFER_FLOW(opengl_init_framebuffer_flow);


/*

Optimization ideas:

*** Sort meshes by materials
*** Sort meshes by their distances from camera (render front to back)

*/

inline opengl_state* GetOpenGL(render_commands* Commands)
{
    opengl_state* Result = (opengl_state*)Commands->StateOfGraphicsAPI;
    
    return(Result);
}

#ifdef INTERNAL_BUILD

void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 
    
    std::cout << "---------------" << std::endl;
    std::cout << "Debug message id = " << id << ": " <<  message << std::endl;
    
    const char* SourceMsg = 0;
    const char* TypeMsg = 0;
    const char* SeverityMsg = 0;
    
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             SourceMsg = "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   SourceMsg = "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: SourceMsg = "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     SourceMsg = "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     SourceMsg = "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           SourceMsg = "Source: Other"; break;
    } 
    
    std::cout << SourceMsg << std::endl;
    
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               TypeMsg = "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: TypeMsg = "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  TypeMsg = "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         TypeMsg = "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         TypeMsg = "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              TypeMsg = "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          TypeMsg = "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           TypeMsg = "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               TypeMsg = "Type: Other"; break;
    } 
    
    std::cout << TypeMsg << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         SeverityMsg = "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       SeverityMsg = "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          SeverityMsg = "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: SeverityMsg = "Severity: notification"; break;
    } 
    
    std::cout << SeverityMsg << std::endl;
    
    std::cout << std::endl;
}

void OpenGL_InitDebugCallbacks()
{
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, 
                          GL_DEBUG_TYPE_ERROR, 
                          GL_DEBUG_SEVERITY_HIGH, 
                          0, nullptr, GL_TRUE);
}

#endif

INTERNAL_FUNCTION void OpenGLCheckError(char* File, int Line)
{
    char* Text = "";
    
    GLenum Error = glGetError();
    
    b32 Print = true;
    switch(Error)
    {
        case GL_NO_ERROR:
        {
            Text = "GL_NO_ERROR\nNo error has been recorded.\n";
            
            Print = false;
        }break;
        
        case GL_INVALID_ENUM:
        {
            Text = "GL_INVALID_ENUM\nAn unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.\n";
        }break;
        
        case GL_INVALID_VALUE:
        {
            Text = "GL_INVALID_VALUE\nA numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.\n";
        }break;
        
        case GL_INVALID_OPERATION:
        {
            Text = "GL_INVALID_OPERATION\nThe specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.\n";
        }break;
        
        case GL_INVALID_FRAMEBUFFER_OPERATION:
        {
            Text = "GL_INVALID_FRAMEBUFFER_OPERATION\nThe framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.\n";
        }break;
        
        case GL_OUT_OF_MEMORY:
        {
            Text = "GL_OUT_OF_MEMORY\nThere is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.\n";
        }break;
        
        case GL_STACK_UNDERFLOW:
        {
            Text = "GL_STACK_UNDERFLOW\nAn attempt has been made to perform an operation that would cause an internal stack to underflow.\n";
        }break;
        
        case GL_STACK_OVERFLOW:
        {
            Text = "GL_STACK_OVERFLOW\nAn attempt has been made to perform an operation that would cause an internal stack to underflow.\n";
        }break;
    }
    
    if(Print)
    {
        char Buf[512];
        
        stbsp_sprintf(Buf, "%s\n(File: %s, Line: %d)\n", Text, File, Line);
        
        printf(Buf);
    }
}

INTERNAL_FUNCTION void OpenGLCheckFramebuffer(char* File, int Line, 
                                              GLenum Target = GL_FRAMEBUFFER)
{
    GLenum Status = glCheckFramebufferStatus(Target);
    
    b32 Print = true;
    char* Text = "";
    
    switch(Status)
    {
        case GL_FRAMEBUFFER_COMPLETE:
        {
            Print = false;
        }break;
        
        case GL_FRAMEBUFFER_UNDEFINED:
        {
            Text = "GL_FRAMEBUFFER_UNDEFINED";
        }break;
        
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        {
            Text = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
        }break;
        
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        {
            Text = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
        }break;
        
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        {
            Text = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
        }break;
        
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        {
            Text = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
        }break;
        
        case GL_FRAMEBUFFER_UNSUPPORTED:
        {
            Text = "GL_FRAMEBUFFER_UNSUPPORTED";
        }break;
        
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        {
            Text = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
        }break;
        
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        {
            Text = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
        }break;
    }
    
    if(Print)
    {
        char Buf[512];
        
        stbsp_sprintf(Buf, "%s\n(File: %s, Line: %d)\n", Text, File, Line);
        
        printf(Buf);
    }
}

INTERNAL_FUNCTION GLuint OpenGL_LoadProgram(char* VertexFilePath, 
                                            char* FragmentFilePath, 
                                            char* GeometryFilePath = 0) 
{
    char* VertexSource = PlatformAPI.ReadFileAndNullTerminate(VertexFilePath);
    char* FragmentSource = PlatformAPI.ReadFileAndNullTerminate(FragmentFilePath);
    char* GeometrySource = PlatformAPI.ReadFileAndNullTerminate(GeometryFilePath);
    
    char InfoLog[1024];
	int Success;
	
	GLuint VertexShader;
	GLuint FragmentShader;
	GLuint GeometryShader;
    GLuint Program;
    
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &VertexSource, 0);
	glCompileShader(VertexShader);
	
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Success);
	if (!Success) {
		glGetShaderInfoLog(VertexShader, sizeof(InfoLog), 0, InfoLog);
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Error while loading vertex shader(%s)\n%s\n", 
                     VertexFilePath, InfoLog);
    }
    
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &FragmentSource, 0);
	glCompileShader(FragmentShader);
    
	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Success);
	if (!Success) {
		glGetShaderInfoLog(FragmentShader, sizeof(InfoLog), 0, InfoLog);
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Error while loading fragment shader(%s)\n%s\n", 
                     FragmentFilePath, InfoLog);
    }
    
    if(GeometrySource){
        GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(GeometryShader, 1, &GeometrySource, 0);
        glCompileShader(GeometryShader);
        
        glGetShaderiv(GeometryShader, GL_COMPILE_STATUS, &Success);
        if (!Success) {
            glGetShaderInfoLog(GeometryShader, sizeof(InfoLog), 0, InfoLog);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, 
                         "Error while loading geometry shader(%s)\n%s\n", 
                         GeometryFilePath, InfoLog);
        }
    }
    
	Program = glCreateProgram();
	glAttachShader(Program, VertexShader);
	glAttachShader(Program, FragmentShader);
	if(GeometrySource){
        glAttachShader(Program, GeometryShader);
    }
    glLinkProgram(Program);
    
	glGetProgramiv(Program, GL_LINK_STATUS, &Success);
	if (!Success)
    {
		glGetProgramInfoLog(Program, sizeof(InfoLog), 0, InfoLog);
		//TODO(dima): Logging
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Error while linking shader program\n%s\n", InfoLog);
        
        Program = -1;
	}
    
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
    if(GeometrySource){
        glDeleteShader(GeometryShader);
    }
    
    free(VertexSource);
    free(FragmentSource);
    if(GeometrySource)
    {
        free(GeometrySource);
    }
    
    return(Program);
}

INTERNAL_FUNCTION opengl_shader* OpenGL_LoadShader(opengl_state* OpenGL,
                                                   char* ShaderName, 
                                                   char* VertexFilePath, 
                                                   char* FragmentFilePath, 
                                                   char* GeometryFilePath = 0)
{
    memory_arena* Arena = OpenGL->Arena;
    opengl_shader* Result = PushStruct(Arena, opengl_shader);
    
    Result->Arena = Arena;
    Result->Type = OpenGL_Shader_Default;
    
    Result->PathV = VertexFilePath;
    Result->PathF = FragmentFilePath;
    Result->PathG = GeometryFilePath;
    
    // NOTE(Dima): Init uniform table
    Result->Name2Loc = hashmap<uniform_name_entry, 256>(Arena);
    Result->Name2Attrib = hashmap<uniform_name_entry, 256>(Arena);
    
    
    Result->ID = OpenGL_LoadProgram(VertexFilePath, 
                                    FragmentFilePath,
                                    GeometryFilePath);
    
    if(Result->ID == -1)
    {
        SDL_Log("%s shader loaded successfully!\n", ShaderName);
    }
    
    CopyStringsSafe(Result->Name, ArrLen(Result->Name), ShaderName);
    
    // NOTE(Dima): Adding this shader to LoadedShaders
    opengl_loaded_shader* LoadedShader = PushStruct(OpenGL->Arena, opengl_loaded_shader);
    LoadedShader->Shader = Result;
    LoadedShader->NextInLoadedShaderList = OpenGL->LoadedShadersList;
    OpenGL->LoadedShadersList = LoadedShader;
    
    return(Result);
}

INTERNAL_FUNCTION void OpenGL_DeleteShader(opengl_shader* Shader)
{
    glDeleteProgram(Shader->ID);
    Shader->ID = -1;
}

INTERNAL_FUNCTION inline b32 ArrayIsValid(GLint Arr){
    b32 Result = 1;
    
    if(Arr == -1){
        Result = 0;
    }
    
    return(Result);
}

INTERNAL_FUNCTION void OpenGLDeleteHandle(renderer_handle* Handle)
{
    if(Handle->Initialized)
    {
        switch(Handle->Type)
        {
            case RendererHandle_Invalid:
            {
                InvalidCodePath;
            }break;
            
            case RendererHandle_Image:
            {
                glDeleteTextures(1, (const GLuint*)&Handle->Image.TextureObject);
                
                Handle->Image.TextureObject = 0;
            }break;
            
            case RendererHandle_Mesh:
            {
                glDeleteVertexArrays(1, (const GLuint*)&Handle->Mesh.ArrayObject);
                glDeleteBuffers(1, (const GLuint*)&Handle->Mesh.BufferObject);
                glDeleteBuffers(1, (const GLuint*)&Handle->Mesh.ElementBufferObject);
                
                Handle->Mesh.ArrayObject = 0;
                Handle->Mesh.BufferObject = 0;
                Handle->Mesh.ElementBufferObject = 0;
            }break;
            
            case RendererHandle_TextureBuffer:
            {
                glDeleteBuffers(1, &Handle->TextureBuffer.BufferObject);
                glDeleteTextures(1, &Handle->TextureBuffer.TextureObject);
                
                Handle->TextureBuffer.BufferObject = 0;
                Handle->TextureBuffer.TextureObject = 0;
            }break;
        }
        
        Handle->Invalidated = false;
        Handle->Initialized = false;
    }
}

INTERNAL_FUNCTION 
b32 OpenGL_ProcessHandleInvalidation(renderer_handle* Handle)
{
    b32 WasDeleted = ShouldDeleteHandleStorage(Handle);
    
    if(WasDeleted)
    {
        OpenGLDeleteHandle(Handle);
        
        Handle->Invalidated = false;
    }
    
    return(WasDeleted);
}

INTERNAL_FUNCTION 
GLuint OpenGL_InitImage(image* Image)
{
    GLuint TexOpenGL = 0;
    
    if (Image)
    {
        b32 ImageWasDeleted = OpenGL_ProcessHandleInvalidation(&Image->Handle);
        
        if(!Image->Handle.Initialized || ImageWasDeleted)
        {
            glGenTextures(1, &TexOpenGL);
            glBindTexture(GL_TEXTURE_2D, TexOpenGL);
            
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         Image->Width,
                         Image->Height,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         Image->Pixels);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            GLuint FilteringMode = GL_LINEAR;
            if(Image->FilteringIsClosest)
            {
                FilteringMode = GL_NEAREST;
            }
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilteringMode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilteringMode);
            
            glBindTexture(GL_TEXTURE_2D, 0);
            
            Image->Handle = CreateRendererHandle(RendererHandle_Image);
            Image->Handle.Image.TextureObject = TexOpenGL;
            Image->Handle.Initialized = true;
        }
        else
        {
            TexOpenGL = Image->Handle.Image.TextureObject;
        }
    }
    
    return(TexOpenGL);
}


INTERNAL_FUNCTION inline void BindImageToCubemapSide(GLuint Target,
                                                     image* Src)
{
    glTexImage2D(Target, 0, GL_RGBA, 
                 Src->Width, 
                 Src->Height, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, 
                 Src->Pixels);
}

INTERNAL_FUNCTION void OpenGLInitCubemap(cubemap* Cubemap)
{
    renderer_handle* CubemapHandle = &Cubemap->Handle;
    
    b32 WasDeleted = OpenGL_ProcessHandleInvalidation(CubemapHandle);
    
    if(!CubemapHandle->Initialized || WasDeleted)
    {
        GLuint NewHandle;
        glGenTextures(1, &NewHandle);
        glBindTexture(GL_TEXTURE_CUBE_MAP, NewHandle);
        
        BindImageToCubemapSide(GL_TEXTURE_CUBE_MAP_POSITIVE_X, Cubemap->Left);
        BindImageToCubemapSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, Cubemap->Right);
        BindImageToCubemapSide(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, Cubemap->Top);
        BindImageToCubemapSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, Cubemap->Down);
        BindImageToCubemapSide(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, Cubemap->Front);
        BindImageToCubemapSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, Cubemap->Back);
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        
        InitRendererHandle(CubemapHandle, RendererHandle_Cubemap);
        CubemapHandle->Cubemap.Handle = NewHandle;
        CubemapHandle->Initialized = true;
    }
}

INTERNAL_FUNCTION void InitAttribFloat(GLint AttrLoc, 
                                       int ComponentCount,
                                       size_t Stride,
                                       size_t Offset)
{
    if(ArrayIsValid(AttrLoc)){
        glEnableVertexAttribArray(AttrLoc);
        glVertexAttribPointer(AttrLoc,
                              ComponentCount, 
                              GL_FLOAT, 
                              GL_FALSE,
                              Stride, 
                              (GLvoid*)(Offset));
    }
}

INTERNAL_FUNCTION void InitAttribInt(GLint AttrLoc,
                                     int ComponentCount,
                                     size_t Stride,
                                     size_t Offset)
{
    if(ArrayIsValid(AttrLoc)){
        glEnableVertexAttribArray(AttrLoc);
        glVertexAttribIPointer(AttrLoc,
                               ComponentCount, 
                               GL_UNSIGNED_INT, 
                               Stride, 
                               (GLvoid*)(Offset));
    }
}

INTERNAL_FUNCTION void OpenGLCreateTextureBuffer(renderer_handle* Handle,
                                                 mi Size,
                                                 void* Data,
                                                 GLuint DataFormat,
                                                 int TextureUnitIndex,
                                                 b32 UsageIsStatic = false)
{
    InitRendererHandle(Handle, RendererHandle_TextureBuffer);
    
    glGenBuffers(1, &Handle->TextureBuffer.BufferObject);
    glGenTextures(1, &Handle->TextureBuffer.TextureObject);
    
    GLuint Usage = GL_STREAM_DRAW;
    if(UsageIsStatic)
    {
        Usage = GL_STATIC_DRAW;
    }
    
    glBindBuffer(GL_TEXTURE_BUFFER, Handle->TextureBuffer.BufferObject);
    glBufferData(GL_TEXTURE_BUFFER, Size, Data, Usage);
    
    glActiveTexture(GL_TEXTURE0 + TextureUnitIndex);
    glBindTexture(GL_TEXTURE_BUFFER, Handle->TextureBuffer.TextureObject);
    glTexBuffer(GL_TEXTURE_BUFFER, DataFormat, Handle->TextureBuffer.BufferObject);
    
    Handle->Initialized = true;
}

INTERNAL_FUNCTION void OpenGLBindTextureBuffer(renderer_handle* Handle,
                                               int TextureUnitIndex,
                                               int UniformLoc)
{
    
    glActiveTexture(GL_TEXTURE0 + TextureUnitIndex);
    glBindTexture(GL_TEXTURE_BUFFER, Handle->TextureBuffer.TextureObject);
    glUniform1i(UniformLoc, TextureUnitIndex);
}

INTERNAL_FUNCTION void OpenGLCreateAndBindTextureBuffer(renderer_handle* Handle,
                                                        mi Size,
                                                        void* Data,
                                                        GLuint DataFormat,
                                                        int TextureUnitIndex,
                                                        int UniformLoc)
{
    OpenGLCreateTextureBuffer(Handle, Size, Data,
                              DataFormat, 
                              TextureUnitIndex,
                              false);
    
    OpenGLBindTextureBuffer(Handle, 
                            TextureUnitIndex,
                            UniformLoc);
}

INTERNAL_FUNCTION inline 
b32 OpenGL_ColorAttachmentIsEmpty(opengl_framebuffer* FB,
                                  int ColorAttachmentIndex)
{
    b32 Empty = FB->ColorTextures[ColorAttachmentIndex] == 0;
    
    return Empty;
}

INTERNAL_FUNCTION
void OpenGL_InitFramebuffer(opengl_framebuffer* FB, 
                            int Width, int Height)
{
    *FB = {};
    
    FB->Resolution = IV2(Width, Height);
    
    // NOTE(Dima): Generating framebuffer
    glGenFramebuffers(1, &FB->Framebuffer);
}

INTERNAL_FUNCTION 
void OpenGL_BindFramebuffer(opengl_framebuffer* FB)
{
    glBindFramebuffer(GL_FRAMEBUFFER, FB->Framebuffer);
    
    glViewport(0, 0, FB->Resolution.Width, FB->Resolution.Height);
}

INTERNAL_FUNCTION
void OpenGL_InitAndBindFramebuffer(opengl_framebuffer* FB,
                                   int Width, int Height)
{
    OpenGL_InitFramebuffer(FB, Width, Height);
    
    OpenGL_BindFramebuffer(FB);
}

INTERNAL_FUNCTION 
void OpenGL_AddColorAttachment(opengl_framebuffer* FB,
                               int ColorAttachmentIndex, 
                               opengl_framebuffer_texture_params TextureParams)
{
    // NOTE(Dima): Generating texture attachment
    glGenTextures(1, &FB->ColorTextures[ColorAttachmentIndex]);
    glBindTexture(GL_TEXTURE_2D, FB->ColorTextures[ColorAttachmentIndex]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureParams.Filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureParams.Filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, 
                 TextureParams.InternalFormat, 
                 FB->Resolution.Width, 
                 FB->Resolution.Height, 
                 0, 
                 TextureParams.Format, 
                 TextureParams.Type, 
                 0);
    
    // NOTE(Dima): Setting texture attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, 
                           GL_COLOR_ATTACHMENT0 + ColorAttachmentIndex, 
                           GL_TEXTURE_2D, 
                           FB->ColorTextures[ColorAttachmentIndex],
                           0);
}

INTERNAL_FUNCTION
void OpenGL_AddDepthAttachment(opengl_framebuffer* FB, b32 ForShadowMap)
{
    // NOTE(Dima): Generating depth texture
    glGenTextures(1, &FB->DepthTexture);
    glBindTexture(GL_TEXTURE_2D, FB->DepthTexture);
    glTexImage2D(GL_TEXTURE_2D,
                 0, 
                 GL_DEPTH_COMPONENT32F,
                 FB->Resolution.Width,
                 FB->Resolution.Height,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT, 0);
    
    if (ForShadowMap)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float BorderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D,
                           FB->DepthTexture, 0);
}

INTERNAL_FUNCTION
void OpenGL_FreeFramebuffer(opengl_framebuffer* FB)
{
    for (int AttachmentIndex = 0;
         AttachmentIndex < MAX_COLOR_ATTACHMENTS;
         AttachmentIndex++)
    {
        if (FB->ColorTextures[AttachmentIndex] != 0)
        {
            glDeleteTextures(1, &FB->ColorTextures[AttachmentIndex]);
            FB->ColorTextures[AttachmentIndex] = 0;
        }
    }
    
    if (FB->DepthTexture != 0)
    {
        glDeleteTextures(1, &FB->DepthTexture);
        FB->DepthTexture = 0;
    }
    
    glDeleteFramebuffers(1, &FB->Framebuffer);
    FB->Framebuffer = 0;
}

INTERNAL_FUNCTION inline 
int OpenGL_GetIndexInPool(framebuffer_in_pool_params Params)
{
    int IndexInPool = Params.Type * FramebufPoolType_Count + Params.Resolution;
    
    return IndexInPool;
}

INTERNAL_FUNCTION inline
framebuffer_in_pool_params OpenGL_GetInPoolParamsFromIndex(int Index)
{
    framebuffer_in_pool_params Result = {};
    
    Result.Type = Index / FramebufPoolType_Count;
    Result.Resolution = Index % FramebufPoolType_Count;
    
    return Result;
}

INTERNAL_FUNCTION 
void OpenGL_InitFramebuffers(render_commands* Commands)
{
    
}

INTERNAL_FUNCTION 
opengl_framebuffer* OpenGL_GetPoolFramebuffer(render_commands* Commands,
                                              framebuffer_in_pool_params Params)
{
    FUNCTION_TIMING();
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    int IndexInPools = OpenGL_GetIndexInPool(Params);
    opengl_framebuffer_pool* Pool = &OpenGL->FramebufferPools[IndexInPools];
    
    if (Pool->NumFree == 0)
    {
        opengl_framebuffer* FB = PushStruct(OpenGL->Arena, opengl_framebuffer);
        
        Pool->FreeFramebuffers[0] = FB;
        Pool->NumFree++;
        
        FB->Resolution = IV2(Pool->Width, Pool->Height);
        FB->IndexInPools = IndexInPools;
        
        // NOTE(Dima): Generating framebuffer
        glGenFramebuffers(1, &FB->Framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, FB->Framebuffer);
        
        
        // NOTE(Dima): Adding color attachment
        opengl_framebuffer_texture_params TextureParams = {};
        
        switch(Params.Type)
        {
            case FramebufPoolType_Color:
            {
                TextureParams.InternalFormat = GL_RGB8;
                TextureParams.Format = GL_RGB;
                TextureParams.Type = GL_UNSIGNED_BYTE;
                TextureParams.Filtering = GL_LINEAR;
            }break;
            
            case FramebufPoolType_HDR:
            {
                TextureParams.InternalFormat = GL_RGB16F;
                TextureParams.Format = GL_RGB;
                TextureParams.Type = GL_FLOAT;
                TextureParams.Filtering = GL_LINEAR;
            }break;
            
            case FramebufPoolType_SSAO:
            {
                TextureParams.InternalFormat = GL_R8;
                TextureParams.Format = GL_RED;
                TextureParams.Type = GL_UNSIGNED_BYTE;
                TextureParams.Filtering = GL_LINEAR;
            }break;
        }
        
        OpenGL_AddColorAttachment(FB, 0, TextureParams);
        
        OpenGLCheckFramebuffer(__FILE__, __LINE__);
    }
    
    // NOTE(Dima): Popping from free stack
    int IndexInFree = Pool->NumFree - 1;
    Pool->NumFree--;
    
    // NOTE(Dima): Appending to Use stack
    int IndexInUse = Pool->NumUse;
    Assert(Pool->NumUse < MAX_FRAMEBUFFERS_IN_FRAMEBUFFER_POOL);
    
    Pool->UseFramebuffers[IndexInUse] = Pool->FreeFramebuffers[IndexInFree];
    Pool->NumUse++;
    
    Pool->FreeFramebuffers[IndexInFree] = 0;
    
    // NOTE(Dima): Returning result
    opengl_framebuffer* Result = Pool->UseFramebuffers[IndexInUse];
    Result->IndexInUse = IndexInUse;
    
    OpenGL_BindFramebuffer(Result);
    
    return(Result);
}

INTERNAL_FUNCTION void OpenGL_ReleasePoolFramebuffer(render_commands* Commands, 
                                                     opengl_framebuffer* FB)
{
    FUNCTION_TIMING();
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    opengl_framebuffer_pool* Pool = &OpenGL->FramebufferPools[FB->IndexInPools];
    
    int IndexOfLastInUse = Pool->NumUse - 1;
    
    Assert(IndexOfLastInUse >= 0);
    
    // NOTE(Dima): Swapping current with last one in the array
    if (IndexOfLastInUse != FB->IndexInUse)
    {
        // NOTE(Dima): Setting a new index_in_use of last element 
        Pool->UseFramebuffers[IndexOfLastInUse]->IndexInUse = FB->IndexInUse;
        
        // NOTE(Dima): Swapping
        opengl_framebuffer* Temp = Pool->UseFramebuffers[IndexOfLastInUse];
        Pool->UseFramebuffers[IndexOfLastInUse] = Pool->UseFramebuffers[FB->IndexInUse];
        Pool->UseFramebuffers[FB->IndexInUse] = Temp;
    }
    
    // NOTE(Dima): Deleting from the end of the array 
    // NOTE(Dima): Deleting from end of Use stack and pushing to end of free stack
    Assert(Pool->NumFree < MAX_FRAMEBUFFERS_IN_FRAMEBUFFER_POOL);
    Pool->FreeFramebuffers[Pool->NumFree++] = Pool->UseFramebuffers[--Pool->NumUse];
    Pool->UseFramebuffers[Pool->NumUse] = 0;
    
    FB->IndexInUse = -1;
}

INTERNAL_FUNCTION void OpenGL_CopyColorFromToFramebuffer(u32 FromFramebuffer,
                                                         iv2 FromRes,
                                                         u32 ToFramebuffer,
                                                         iv2 ToRes)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ToFramebuffer);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FromFramebuffer);
    
    glBlitFramebuffer(0, 0, FromRes.w, FromRes.h,
                      0, 0, ToRes.w, ToRes.h,
                      GL_COLOR_BUFFER_BIT,
                      GL_LINEAR);
}

INTERNAL_FUNCTION opengl_framebuffer* OpenGL_DoDilation(render_commands* Commands, 
                                                        u32 InputTexture,
                                                        char* PostProcEffectGUID)
{
    FUNCTION_TIMING();
    
    dilation_params* Params = PostProcEffect_GetParams(&Commands->PostProcessing,
                                                       PostProcEffectGUID, dilation_params);
    
    // NOTE(Dima): Getting framebuffer for post-process effect
    framebuffer_in_pool_params InPoolParams = {};
    InPoolParams.Resolution = Params->Resolution;
    InPoolParams.Type = FramebufPoolType_Color;
    
    opengl_framebuffer* Result = OpenGL_GetPoolFramebuffer(Commands, InPoolParams);
    
    // NOTE(Dima): Apply shader
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->DilationShader;
    
    Shader->Use();
    Shader->SetInt("Size", Params->Size);
    Shader->SetVec2("MinMaxThreshold", 
                    Params->MinThreshold, 
                    Params->MaxThreshold);
    Shader->SetTexture2D("InputTexture", InputTexture, 0);
    
    // NOTE(Dima): Render
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    return(Result);
}

INTERNAL_FUNCTION opengl_framebuffer* OpenGL_DoPosterize(render_commands* Commands,
                                                         u32 InputTexture,
                                                         char* PostProcEffectGUID)
{
    FUNCTION_TIMING();
    
    posterize_params* Params = PostProcEffect_GetParams(&Commands->PostProcessing,
                                                        PostProcEffectGUID, posterize_params);
    
    // NOTE(Dima): Getting framebuffer for post-process effect
    framebuffer_in_pool_params InPoolParams = {};
    InPoolParams.Resolution = Params->Resolution;
    InPoolParams.Type = FramebufPoolType_Color;
    
    opengl_framebuffer* Result = OpenGL_GetPoolFramebuffer(Commands, InPoolParams);
    
    // NOTE(Dima): Apply shader
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->PosterizeShader;
    
    Shader->Use();
    Shader->SetInt("Levels", Params->Levels);
    Shader->SetTexture2D("InputTexture", InputTexture, 0);
    
    // NOTE(Dima): Render
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    return(Result);
}

INTERNAL_FUNCTION opengl_framebuffer* OpenGL_DoCrtDisplay(render_commands* Commands,
                                                          u32 InputTexture,
                                                          char* PostProcEffectGUID)
{
    FUNCTION_TIMING();
    
    crt_display_params* Params = PostProcEffect_GetParams(&Commands->PostProcessing,
                                                          PostProcEffectGUID, 
                                                          crt_display_params);
    
    // NOTE(Dima): Getting framebuffer for post-process effect
    framebuffer_in_pool_params InPoolParams = {};
    InPoolParams.Resolution = Params->Resolution;
    InPoolParams.Type = FramebufPoolType_Color;
    
    opengl_framebuffer* Result = OpenGL_GetPoolFramebuffer(Commands, InPoolParams);
    
    // NOTE(Dima): Apply shader
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->CrtDisplayShader;
    
    Shader->Use();
    Shader->SetTexture2D("InputTexture", InputTexture, 0);
    Shader->SetVec2("DstTextureSize", V2(Result->Resolution));
    Shader->SetVec2("Curvature", Params->Curvature);
    Shader->SetVec2("ScanLineOpacity", Params->ScanLineOpacity);
    Shader->SetFloat("CellSize", Params->CellSize);
    Shader->SetBool("VignetteEnabled", Params->VignetteEnabled);
    Shader->SetFloat("VignetteRoundness", Params->VignetteRoundness);
    Shader->SetFloat("VignetteOpacity", Params->VignetteOpacity);
    Shader->SetFloat("VignetteBrightnessCompensation", Params->VignetteBrightnessCompensation);
    Shader->SetFloat("VignettePower", Params->VignettePower);
    
    // NOTE(Dima): Render
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    return(Result);
}

INTERNAL_FUNCTION opengl_framebuffer* OpenGL_DoBoxBlur(render_commands* Commands,
                                                       u32 InputTexture,
                                                       char* PostProcEffectGUID)
{
    FUNCTION_TIMING();
    
    box_blur_params* Params = PostProcEffect_GetParams(&Commands->PostProcessing,
                                                       PostProcEffectGUID, box_blur_params);
    
    // NOTE(Dima): Getting framebuffer for post-process effect
    framebuffer_in_pool_params InPoolParams = {};
    InPoolParams.Resolution = Params->Resolution;
    InPoolParams.Type = FramebufPoolType_Color;
    
    opengl_framebuffer* Result = OpenGL_GetPoolFramebuffer(Commands, InPoolParams);
    
    int RadiusSize = Params->RadiusSize;
    
    if (Params->EnableCheapMode)
    {
        // TODO(Dima): 
        // NOTE(Dima): If we use half-res or quater-res textures then we will get blur for free
        if(Params->Resolution != DownscaleRes_1)
        {
            RadiusSize = 0;
        }
    }
    
    // NOTE(Dima): Apply shader
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->BoxBlurShader;
    
    Shader->Use();
    Shader->SetInt("BlurRadius", RadiusSize);
    Shader->SetTexture2D("ToBlurTex", InputTexture, 0);
    
    // NOTE(Dima): Render
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    return(Result);
}

INTERNAL_FUNCTION opengl_framebuffer* OpenGL_DoDepthOfField(render_commands* Commands,
                                                            u32 InputFocus,
                                                            u32 InputOutOfFocus,
                                                            u32 DepthTexture,
                                                            char* PostProcEffectGUID)
{
    FUNCTION_TIMING();
    
    // NOTE(Dima): Getting framebuffer for post-process effect
    
    framebuffer_in_pool_params InPoolParams = {};
    InPoolParams.Resolution = DownscaleRes_1;
    InPoolParams.Type = FramebufPoolType_Color;
    
    opengl_framebuffer* Result = OpenGL_GetPoolFramebuffer(Commands, InPoolParams);
    
    dof_params* Params = PostProcEffect_GetParams(&Commands->PostProcessing,
                                                  PostProcEffectGUID,
                                                  dof_params);
    
    const m44& Projection = Commands->RenderPasses[0].Projection;
    
    // NOTE(Dima): Apply shader
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->DepthOfFieldShader;
    
    Shader->Use();
    Shader->SetFloat("MinDistance", Params->MinDistance);
    Shader->SetFloat("MaxDistance", Params->MaxDistance);
    Shader->SetFloat("FocusZ", Params->FocusZ);
    Shader->SetVec2("WH", 
                    Commands->WindowDimensions.Current.Width,
                    Commands->WindowDimensions.Current.Height);
    Shader->SetVec4("PerspProjCoefs",
                    Projection.e[0],
                    Projection.e[5],
                    Projection.e[10],
                    Projection.e[14]);
    
    Shader->SetTexture2D("FocusTex", InputFocus, 0);
    Shader->SetTexture2D("OutOfFocusTex", InputOutOfFocus, 1);
    Shader->SetTexture2D("DepthTex", DepthTexture, 2);
    
    // NOTE(Dima): Render
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    return(Result);
}

INTERNAL_FUNCTION inline
b32 OpenGL_NeedResizeFramebuffer(opengl_framebuffer* Framebuffer,
                                 rt_downscale_res DownscaleRes,
                                 iv2 BaseResolution,
                                 iv2* NewResolution)
{
    int Divisor = 1;
    switch(DownscaleRes)
    {
        case DownscaleRes_1:
        {
            Divisor = 1;
        }break;
        
        case DownscaleRes_1div2:
        {
            Divisor = 2;
        }break;
        
        case DownscaleRes_1div4:
        {
            Divisor = 4;
        }break;
        
#if 0
        case DownscaleRes_1div8:
        {
            Divisor = 8;
        }break;
#endif
        
    }
    
    iv2 Resolution = BaseResolution / Divisor;
    *NewResolution = Resolution;
    b32 Result = Resolution != Framebuffer->Resolution;
    
    return Result;
}

INTERNAL_FUNCTION
void OpenGL_PreProcessFramebuffer(opengl_framebuffer* Framebuffer,
                                  rt_downscale_res DownscaleRes,
                                  iv2 BaseResolution,
                                  opengl_init_framebuffer_flow* InitFramebufferFlow)
{
    iv2 NewDim;
    if (OpenGL_NeedResizeFramebuffer(Framebuffer, 
                                     DownscaleRes, 
                                     BaseResolution,
                                     &NewDim))
    {
        // NOTE(Dima): Freeing framebuffer
        OpenGL_FreeFramebuffer(Framebuffer);
        
        // NOTE(Dima): Initializing framebuffer
        InitFramebufferFlow(Framebuffer, NewDim.Width, NewDim.Height);
    }
}

INTERNAL_FUNCTION OPENGL_INIT_FRAMEBUFFER_FLOW(OpenGL_InitFlowBufferSSAO)
{
    opengl_framebuffer_texture_params TextureParams = {};
    TextureParams.InternalFormat = GL_R8;
    TextureParams.Format = GL_RED;
    TextureParams.Type = GL_UNSIGNED_BYTE;
    TextureParams.Filtering = GL_LINEAR;
    
    OpenGL_InitAndBindFramebuffer(Buffer, Width, Height);
    OpenGL_AddColorAttachment(Buffer, 0, TextureParams);
}

INTERNAL_FUNCTION void OpenGL_InitSSAO(render_commands* Commands,
                                       int Width, int Height)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    post_processing* PP = &Commands->PostProcessing;
    
    // NOTE(Dima): init SSAO noise texture
    glGenTextures(1, &OpenGL->SSAONoiseTex);
    glBindTexture(GL_TEXTURE_2D, OpenGL->SSAONoiseTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &PP->SSAO_Noise);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

INTERNAL_FUNCTION void OpenGL_FreeSSAO(render_commands* Commands)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    glDeleteTextures(1, &OpenGL->SSAONoiseTex);
    
    // NOTE(Dima): Freing SSAO Blur Framebuffer
    OpenGL_FreeFramebuffer(&OpenGL->SSAOBlurBuffer1);
    OpenGL_FreeFramebuffer(&OpenGL->SSAOBlurBuffer2);
    
    // NOTE(Dima): Freing SSAO Framebuffer
    OpenGL_FreeFramebuffer(&OpenGL->SSAOBuffer);
}

INTERNAL_FUNCTION
u32 OpenGL_DoBlurSSAO(render_commands* Commands, 
                      opengl_framebuffer* BlurBuffer, 
                      ssao_params* Params,
                      render_pass* RenderPass,
                      b32 IsHorizontal,
                      u32 InTexture)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    post_processing* PP = &Commands->PostProcessing;
    opengl_shader* Shader = OpenGL->SSAOBlurShader;
    
    OpenGL_PreProcessFramebuffer(BlurBuffer,
                                 Params->BlurResolution,
                                 IV2(RenderPass->Width, RenderPass->Height),
                                 OpenGL_InitFlowBufferSSAO);
    
    OpenGL_BindFramebuffer(BlurBuffer);
    
    // NOTE(Dima): Setting shader params
    Shader->Use();
    Shader->SetTexture2D("OcclusionTex", InTexture, 0);
    Shader->SetInt("BlurRadius", Params->BlurRadius);
    Shader->SetBool("IsHorizontalPass", IsHorizontal);
    
    Assert(Params->BlurRadius < ArrLen(PP->Gaussian1DKernelForRadius));
    Shader->SetFloatArray("GaussianKernel", 
                          PP->Gaussian1DKernelForRadius[Params->BlurRadius], 
                          Params->BlurRadius + 1);
    
    // NOTE(Dima): Rendering
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    u32 Result = BlurBuffer->ColorTextures[0];
    
    return Result;
}

INTERNAL_FUNCTION 
u32 OpenGL_DoSSAO(render_commands* Commands, 
                  render_pass* RenderPass,
                  u32 NormalTex,
                  u32 DepthTex,
                  char* PostProcSSAO_GUID)
{
    FUNCTION_TIMING();
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    post_processing* PP = &Commands->PostProcessing;
    
    const m44& Projection = RenderPass->Projection;
    
    ssao_params* Params = PostProcEffect_GetParams(&Commands->PostProcessing,
                                                   PostProcSSAO_GUID, ssao_params);
    
    OpenGL_PreProcessFramebuffer(&OpenGL->SSAOBuffer,
                                 Params->Resolution,
                                 IV2(RenderPass->Width, RenderPass->Height),
                                 OpenGL_InitFlowBufferSSAO);
    
    OpenGL_BindFramebuffer(&OpenGL->SSAOBuffer);
    
    // NOTE(Dima): Setting shader params
    opengl_shader* Shader = OpenGL->SSAOShader;
    
    Shader->Use();
    Shader->SetTexture2D("DepthTex", DepthTex, 0);
    Shader->SetTexture2D("NormalTex", NormalTex, 1);
    Shader->SetTexture2D("SSAONoiseTex", OpenGL->SSAONoiseTex, 2);
    
    Shader->SetVec3Array("SSAOKernel", 
                         PP->SSAO_Kernel, 
                         Params->KernelSize);
    Shader->SetInt("SSAOKernelSamplesCount", Params->KernelSize);
    Shader->SetFloat("SSAOKernelRadius", Params->KernelRadius);
    Shader->SetFloat("SSAORangeCheck", Params->RangeCheck);
    Shader->SetVec2("WH", 
                    OpenGL->SSAOBuffer.Resolution.Width,
                    OpenGL->SSAOBuffer.Resolution.Height);
    Shader->SetVec4("PerspProjCoefs",
                    Projection.e[0],
                    Projection.e[5],
                    Projection.e[10],
                    Projection.e[14]);
    Shader->SetMat4("View", RenderPass->View.e);
    
    u32 ResultTextureSSAO = OpenGL->SSAOBuffer.ColorTextures[0];
    
    // NOTE(Dima): Rendering
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // NOTE(Dima): SSAO Blur pass if enabled
    if (Params->BlurEnabled)
    {
        ResultTextureSSAO = OpenGL_DoBlurSSAO(Commands, 
                                              &OpenGL->SSAOBlurBuffer1, 
                                              Params, 
                                              RenderPass,
                                              true,
                                              OpenGL->SSAOBuffer.ColorTextures[0]);
        
        ResultTextureSSAO = OpenGL_DoBlurSSAO(Commands,
                                              &OpenGL->SSAOBlurBuffer2, 
                                              Params, 
                                              RenderPass,
                                              false,
                                              ResultTextureSSAO);
        
    }
    
    return ResultTextureSSAO;
}

INTERNAL_FUNCTION OPENGL_INIT_FRAMEBUFFER_FLOW(OpenGL_InitFlowGBuffer)
{
    FUNCTION_TIMING();
    
    OpenGL_InitAndBindFramebuffer(Buffer, Width, Height);
    
    // NOTE(Dima): Init ColorSpec texture
    opengl_framebuffer_texture_params ColorSpecParams = {};
    ColorSpecParams.InternalFormat = GL_RGBA8;
    ColorSpecParams.Format = GL_RGBA;
    ColorSpecParams.Type = GL_UNSIGNED_BYTE;
    ColorSpecParams.Filtering = GL_NEAREST;
    
    OpenGL_AddColorAttachment(Buffer, 
                              GBufferTex_Colors, 
                              ColorSpecParams);
    
    
    // NOTE(Dima): Init normal texture
    opengl_framebuffer_texture_params NormalParams = {};
    NormalParams.InternalFormat = GL_RGB16F;
    NormalParams.Format = GL_RGB;
    NormalParams.Type = GL_FLOAT;
    NormalParams.Filtering = GL_NEAREST;
    
    OpenGL_AddColorAttachment(Buffer, 
                              GBufferTex_Normals, 
                              NormalParams);
    
    //NOTE(Dima): Init positions texture
    opengl_framebuffer_texture_params PosParams = {};
    PosParams.InternalFormat = GL_RGB32F;
    PosParams.Format = GL_RGB;
    PosParams.Type = GL_FLOAT;
    PosParams.Filtering = GL_NEAREST;
    
    OpenGL_AddColorAttachment(Buffer, 
                              GBufferTex_Positions, 
                              PosParams);
    
    // NOTE(Dima): Setting draw buffers
    u32 Attachments[] = {
        GL_COLOR_ATTACHMENT0, 
        GL_COLOR_ATTACHMENT1, 
        GL_COLOR_ATTACHMENT2, 
    };
    glDrawBuffers(ArrLen(Attachments), Attachments);
    
    // NOTE(Dima): Init Depth texture
    OpenGL_AddDepthAttachment(Buffer, false);
}

INTERNAL_FUNCTION void OpenGL_InitGBuffer(render_commands* Commands,
                                          int Width, int Height)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    OpenGL_InitFlowGBuffer(&OpenGL->GBuffer, Width, Height);
}

INTERNAL_FUNCTION opengl_array_object OpenGL_BeginScreenQuad(v2 At, v2 Dim, 
                                                             u32 Usage = GL_STATIC_DRAW)
{
    opengl_array_object Result = {};
    
    v2 Min = At;
    v2 Max = At + Dim;
    
    // NOTE(Dima): Init screen Quad
    f32 QuadData[] = 
    {
        Min.x, Max.y, 0.0f, 1.0f,
        Max.x, Max.y, 1.0f, 1.0f,
        Max.x, Min.y, 1.0f, 0.0f,
        
        Min.x, Max.y, 0.0f, 1.0f,
        Max.x, Min.y, 1.0f, 0.0f,
        Min.x, Min.y, 0.0f, 0.0f,
    };
    
    glGenVertexArrays(1, &Result.VAO);
    glGenBuffers(1, &Result.VBO);
    
    glBindVertexArray(Result.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Result.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadData), QuadData, Usage);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, 0, 4 * sizeof(float), 0);
    glBindVertexArray(0);
    
    return(Result);
}

INTERNAL_FUNCTION void OpenGL_DeleteArrayObject(opengl_array_object* Obj)
{
    glDeleteVertexArrays(1, &Obj->VAO);
    glDeleteBuffers(1, &Obj->VBO);
    glDeleteBuffers(1, &Obj->EBO);
}

INTERNAL_FUNCTION void OpenGLInitDefaultObjects(render_commands* Commands)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    OpenGL->ScreenQuad = OpenGL_BeginScreenQuad(V2(-1.0f), V2(2.0f), GL_STATIC_DRAW);
    
    // NOTE(Dima): Init skybox cube
    float SkyboxVertices[] = 
    {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    
    glGenVertexArrays(1, &OpenGL->SkyboxCube.VAO);
    glGenBuffers(1, &OpenGL->SkyboxCube.VBO);
    
    glBindVertexArray(OpenGL->SkyboxCube.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->SkyboxCube.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SkyboxVertices), SkyboxVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 3 * sizeof(float), 0);
    glBindVertexArray(0);
}

INTERNAL_FUNCTION void OpenGL_InitShadowMaps(render_commands* Commands)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    lighting* Lighting = &Commands->Lighting;
    
    opengl_framebuffer* FB = &OpenGL->ShadowMapsFramebuffer;
    OpenGL_InitAndBindFramebuffer(FB, 
                                  Lighting->ShadowMapRes,
                                  Lighting->ShadowMapRes);
    
    OpenGL->InitCascadesCount = Lighting->CascadeCount;
    
    // NOTE(Dima): Generate depth texture array
    glGenTextures(1, &FB->ColorTextures[0]);
    glBindTexture(GL_TEXTURE_2D_ARRAY, FB->ColorTextures[0]);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY,
                   1, 
                   GL_RG32F,
                   Lighting->ShadowMapRes,
                   Lighting->ShadowMapRes,
                   Lighting->CascadeCount + 1); // +1 temp
    
    // NOTE(Dima): Setting params
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float BorderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, BorderColor);
    
    // NOTE(Dima): Creating depth texture
    OpenGL_AddDepthAttachment(FB, true);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    OpenGLCheckError(__FILE__, __LINE__);
    OpenGLCheckFramebuffer(__FILE__, __LINE__);
}

INTERNAL_FUNCTION void OpenGL_FreeShadowMaps(render_commands* Commands)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    lighting* Lighting = &Commands->Lighting;
    
    opengl_framebuffer* ShadowMapFramebuffer = &OpenGL->ShadowMapsFramebuffer;
    
    OpenGL_FreeFramebuffer(ShadowMapFramebuffer);
}

INTERNAL_FUNCTION void OpenGL_LoadShaders(render_commands* Commands, memory_arena* Arena)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    // NOTE(Dima): Init shaders
    OpenGL->StdShader = OpenGL_LoadShader(OpenGL, "Standard",
                                          "../Data/Shaders/std.vs",
                                          "../Data/Shaders/std.fs");
    
    OpenGL->StdShadowShader = OpenGL_LoadShader(OpenGL, "StandardShadow",
                                                "../Data/Shaders/std.vs",
                                                "../Data/Shaders/shadow_depth.fs");
    
    OpenGL->UIRectShader = OpenGL_LoadShader(OpenGL, "UIRect",
                                             "../Data/Shaders/ui_rect.vs",
                                             "../Data/Shaders/ui_rect.fs");
    
    OpenGL->VoxelShader = OpenGL_LoadShader(OpenGL, "Voxel",
                                            "../Data/Shaders/voxel.vs",
                                            "../Data/Shaders/voxel.fs");
    
    OpenGL->VoxelShadowShader = OpenGL_LoadShader(OpenGL, "VoxelShadow",
                                                  "../Data/Shaders/voxel.vs",
                                                  "../Data/Shaders/shadow_depth.fs");
    
    OpenGL->SSAOShader = OpenGL_LoadShader(OpenGL, "SSAO",
                                           "../Data/Shaders/screen.vs",
                                           "../Data/Shaders/ssao.fs");
    
    OpenGL->SSAOBlurShader = OpenGL_LoadShader(OpenGL, "SSAOBlur",
                                               "../Data/Shaders/screen.vs",
                                               "../Data/Shaders/ssao_blur.fs");
    
    OpenGL->LightingShader = OpenGL_LoadShader(OpenGL, "Lighting",
                                               "../Data/Shaders/screen.vs",
                                               "../Data/Shaders/lighting.fs");
    
    OpenGL->BoxBlurShader = OpenGL_LoadShader(OpenGL, "BoxBlur",
                                              "../Data/Shaders/screen.vs",
                                              "../Data/Shaders/box_blur.fs");
    
    OpenGL->DilationShader = OpenGL_LoadShader(OpenGL, "Dilation",
                                               "../Data/Shaders/screen.vs",
                                               "../Data/Shaders/dilation.fs");
    
    OpenGL->PosterizeShader = OpenGL_LoadShader(OpenGL, "Posterize",
                                                "../Data/Shaders/screen.vs",
                                                "../Data/Shaders/posterize.fs");
    
    OpenGL->DepthOfFieldShader = OpenGL_LoadShader(OpenGL, "DepthOfField",
                                                   "../Data/Shaders/screen.vs",
                                                   "../Data/Shaders/depth_of_field.fs");
    
    OpenGL->DepthOfFieldShader = OpenGL_LoadShader(OpenGL, "DepthOfField",
                                                   "../Data/Shaders/screen.vs",
                                                   "../Data/Shaders/depth_of_field.fs");
    
    OpenGL->SkyShader = OpenGL_LoadShader(OpenGL, "Sky",
                                          "../Data/Shaders/sky.vs",
                                          "../Data/Shaders/sky.fs");
    
    OpenGL->RenderDepthShader = OpenGL_LoadShader(OpenGL, "RenderDepth",
                                                  "../Data/Shaders/screen.vs",
                                                  "../Data/Shaders/render_depth.fs");
    
    OpenGL->VarianceShadowBlurShader = OpenGL_LoadShader(OpenGL, "VarianceShadowmapBlur",
                                                         "../Data/Shaders/screen.vs",
                                                         "../Data/Shaders/variance_shadowmap_blur.fs");
    
    OpenGL->RenderWaterShader = OpenGL_LoadShader(OpenGL, "RenderWater",
                                                  "../Data/Shaders/screen.vs",
                                                  "../Data/Shaders/render_water.fs");
    
    OpenGL->CrtDisplayShader = OpenGL_LoadShader(OpenGL, "CrtDisplay",
                                                 "../Data/Shaders/screen.vs",
                                                 "../Data/Shaders/crt_display.fs");
}

INTERNAL_FUNCTION 
void OpenGL_InitFramebufferPool(render_commands* Commands,
                                framebuffer_in_pool_params Params)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    int IndexInPool = OpenGL_GetIndexInPool(Params);
    opengl_framebuffer_pool* Result = &OpenGL->FramebufferPools[IndexInPool];
    
    f32 ResolutionDivisor = Global_PostProcResolutionDivisors[Params.Resolution];
    
    // NOTE(Dima): Init the result
    Result->Width = (f32)Commands->WindowDimensions.Init.Width / (f32)ResolutionDivisor;
    Result->Height = (f32)Commands->WindowDimensions.Init.Height / (f32)ResolutionDivisor;
    
    Result->NumUse = 0;
    Result->NumFree = 0;
}

// NOTE(Dima): Check if framebuffer pool does not contain any framebuffers in Use
INTERNAL_FUNCTION inline b32 OpenGL_PoolUseIsEmpty(opengl_framebuffer_pool* Pool)
{
    b32 PoolIsEmpty = Pool->NumUse == 0;
    
    return PoolIsEmpty;
}

INTERNAL_FUNCTION void OpenGL_DeleteFramebufferPool(opengl_framebuffer_pool* Pool)
{
    Assert(Pool->NumUse == 0);
    Assert(OpenGL_PoolUseIsEmpty(Pool));
    
    // NOTE(Dima): Delete framebuffers
    for(int Index = 0;
        Index < Pool->NumFree;
        Index++)
    {
        opengl_framebuffer* FB = Pool->FreeFramebuffers[Index];
        
        OpenGL_FreeFramebuffer(FB);
    }
}

INTERNAL_FUNCTION b32 OpenGL_PoolsAreEmptyCheck(render_commands* Commands)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    b32 Result = true;
    
    for (int PoolIndex = 0;
         PoolIndex < NUM_FRAMEBUFFER_POOLS;
         PoolIndex++)
    {
        opengl_framebuffer_pool* Pool = &OpenGL->FramebufferPools[PoolIndex];
        
        Result = Result && OpenGL_PoolUseIsEmpty(Pool);
    }
    
    // NOTE(Dima): When this function is called at the end of the frame, it's result should be true
    // NOTE(Dima): Because if not, it means that we'll have a mismatched Begin/End use of FB
    Assert(Result);
    
    return Result;
}

INTERNAL_FUNCTION void OpenGL_Init(render_commands* Commands, memory_arena* Arena)
{
    opengl_state* OpenGL = PushStruct(Commands->Arena, opengl_state);
    Commands->StateOfGraphicsAPI = OpenGL;
    OpenGL->Arena = Arena;
    
    int Width = Commands->WindowDimensions.Init.Width;
    int Height = Commands->WindowDimensions.Init.Height;
    lighting* Lighting = &Commands->Lighting;
    
    glewExperimental = GL_TRUE;
    glewInit();
    
    
#ifdef INTERNAL_BUILD
    OpenGL_InitDebugCallbacks();
#endif
    
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 
                  &OpenGL->MaxCombinedTextureUnits);
    
    
    SDL_GL_SetSwapInterval(0);
    
    glEnable(GL_DEPTH_TEST);
    
    OpenGLInitDefaultObjects(Commands);
    
    // NOTE(Dima): Init framebuffer pools
    for(int PoolIndex = 0;
        PoolIndex< NUM_FRAMEBUFFER_POOLS;
        PoolIndex++)
    {
        framebuffer_in_pool_params InPoolParams = OpenGL_GetInPoolParamsFromIndex(PoolIndex);
        
        OpenGL_InitFramebufferPool(Commands, InPoolParams);
    }
    
    // NOTE(Dima): Init GBuffer
    OpenGL_InitGBuffer(Commands, Width, Height);
    OpenGL_InitSSAO(Commands, Width, Height);
    OpenGL_InitShadowMaps(Commands);
    
    OpenGL_LoadShaders(Commands, Arena);
    
    OpenGLCheckError(__FILE__, __LINE__);
}

INTERNAL_FUNCTION void OpenGL_Free(render_commands* Commands)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    // NOTE(Dima): Free objects
    OpenGL_DeleteArrayObject(&OpenGL->ScreenQuad);
    OpenGL_DeleteArrayObject(&OpenGL->SkyboxCube);
    
    // NOTE(Dima): Delete GBuffer
    OpenGL_FreeFramebuffer(&OpenGL->GBuffer);
    OpenGL_FreeSSAO(Commands);
    
    // NOTE(Dima): Deleting all framebuffer pools
    for (int PoolIndex = 0;
         PoolIndex < NUM_FRAMEBUFFER_POOLS;
         PoolIndex++)
    {
        OpenGL_DeleteFramebufferPool(&OpenGL->FramebufferPools[PoolIndex]);
    }
    
    // NOTE(Dima): Delete shaders
    opengl_loaded_shader* ShaderAt = OpenGL->LoadedShadersList;
    while(ShaderAt != 0)
    {
        OpenGL_DeleteShader(ShaderAt->Shader);
        
        ShaderAt = ShaderAt->NextInLoadedShaderList;
    }
}

INTERNAL_FUNCTION void OpenGL_InitMeshAttribs(mesh* Mesh, opengl_shader* Shader, b32 IsShadowShader)
{
    int CurrentProgramID = opengl_shader::GetCurrentProgram();
    
    Shader->Use();
    
    // NOTE(Dima): Position
    InitAttribFloat(Shader->GetAttribLoc("InPosition"),
                    3, 3 * sizeof(float),
                    Mesh->Offsets.OffsetP);
    
    if (!IsShadowShader)
    {
        // NOTE(Dima): TexCoords
        InitAttribFloat(Shader->GetAttribLoc("InTexCoords"),
                        2, 2 * sizeof(float),
                        Mesh->Offsets.OffsetUV);
        
        // NOTE(Dima): Normal
        InitAttribFloat(Shader->GetAttribLoc("InNormal"),
                        3, 3 * sizeof(float),
                        Mesh->Offsets.OffsetN);
        
        // NOTE(Dima): Color
        InitAttribInt(Shader->GetAttribLoc("InColor"),
                      1, sizeof(u32),
                      Mesh->Offsets.OffsetC);
    }
    
    
    // NOTE(Dima): Setting skinning info
    if(Mesh->IsSkinned)
    {
        // NOTE(Dima): Bone weights
        InitAttribFloat(Shader->GetAttribLoc("InWeights"),
                        4, 4 * sizeof(float),
                        Mesh->Offsets.OffsetBoneWeights);
        
        // NOTE(Dima): Bone indices
        InitAttribFloat(Shader->GetAttribLoc("InBoneIDs"),
                        1, sizeof(u32),
                        Mesh->Offsets.OffsetBoneIndices);
    }
    
    if(CurrentProgramID != 0)
    {
        glUseProgram(CurrentProgramID);
    }
}

INTERNAL_FUNCTION renderer_handle* 
OpenGL_AllocateMesh(opengl_state* OpenGL,
                    mesh* Mesh)
{
    renderer_handle* Result = &Mesh->Handle;
    
    b32 MeshWasDeleted = OpenGL_ProcessHandleInvalidation(Result);
    
    if(!Result->Initialized || MeshWasDeleted)
    {
        if(Mesh->MeshDataSize > 0)
        {
            
            GLuint VAO = 0;
            GLuint VBO = 0;
            GLuint EBO = 0;
            
            // NOTE(Dima): Init VAO and VBO
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            
            glBindVertexArray(VAO);
            
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, 
                         Mesh->MeshDataSize, 
                         Mesh->MeshDataStart, 
                         GL_STATIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                         sizeof(u32) * Mesh->IndexCount, 
                         Mesh->Indices, 
                         GL_STATIC_DRAW);
            
            OpenGL_InitMeshAttribs(Mesh, OpenGL->StdShader, false);
            OpenGL_InitMeshAttribs(Mesh, OpenGL->StdShadowShader, true); 
            
            //glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            
            Mesh->Handle = CreateRendererHandle(RendererHandle_Mesh);
            Result->Mesh.ArrayObject = VAO;
            Result->Mesh.BufferObject = VBO;
            Result->Mesh.ElementBufferObject = EBO;
            Result->Initialized = true;
        }
    }
    
    return(Result);
}

INTERNAL_FUNCTION void OpenGL_RenderMesh(render_commands* Commands,
                                         opengl_shader* Shader,
                                         render_pass* RenderPass,
                                         mesh* Mesh,
                                         material* Material,
                                         v3 Color,
                                         m44* SkinningMatrices,
                                         int NumInstanceSkMat,
                                         m44* InstanceModelTransforms,
                                         int MeshInstanceCount,
                                         b32 UseInstancing)
{
    FUNCTION_TIMING();
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    OpenGL_AllocateMesh(OpenGL, Mesh);
    
    // NOTE(Dima): Beginning use of shader
    Shader->Use();
    
    Shader->SetMat4("ViewProjection", RenderPass->ViewProjection);
    Shader->SetMat4("View", RenderPass->View.e);
    Shader->SetMat4("Model", InstanceModelTransforms[0].e);
    Shader->SetVec3("MultColor", Color.r, Color.g, Color.b);
    Shader->SetBool("UseInstancing", UseInstancing);
    
    
    // NOTE(Dima): Render
    glBindVertexArray(Mesh->Handle.Mesh.ArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, Mesh->Handle.Mesh.BufferObject);
    
    // NOTE(Dima): Instancing
    int ModelTranLoc1;
    int ModelTranLoc2;
    int ModelTranLoc3;
    int ModelTranLoc4;
    
    GLuint InstanceModelBO;
    if(UseInstancing)
    {
        ModelTranLoc1 = Shader->GetAttribLoc("InInstanceModelTran1");
        ModelTranLoc2 = Shader->GetAttribLoc("InInstanceModelTran2");
        ModelTranLoc3 = Shader->GetAttribLoc("InInstanceModelTran3");
        ModelTranLoc4 = Shader->GetAttribLoc("InInstanceModelTran4");
        
        glGenBuffers(1, &InstanceModelBO);
        
        // NOTE(Dima): Generating buffer for holding instance model transforms and mesh
        glBindBuffer(GL_ARRAY_BUFFER, InstanceModelBO);
        glBufferData(GL_ARRAY_BUFFER, 
                     sizeof(m44) * MeshInstanceCount + Mesh->MeshDataSize, 
                     0,
                     GL_STREAM_DRAW);
        
        // NOTE(Dima): Adding mesh to buffer
        if(Mesh->MeshDataSize > 0)
        {
            glBufferSubData(GL_ARRAY_BUFFER,
                            0,
                            Mesh->MeshDataSize,
                            Mesh->MeshDataStart);
        }
        
        // NOTE(Dima): Adding instance model transforms to buffer
        if(MeshInstanceCount)
        {
            glBufferSubData(GL_ARRAY_BUFFER,
                            Mesh->MeshDataSize,
                            sizeof(m44) * MeshInstanceCount,
                            InstanceModelTransforms);
        }
        
        OpenGL_InitMeshAttribs(Mesh, OpenGL->StdShader, false);
        OpenGL_InitMeshAttribs(Mesh, OpenGL->StdShadowShader, true);
        
        // NOTE(Dima): Setting instance model transform attribs
        InitAttribFloat(ModelTranLoc1,
                        4, sizeof(m44),
                        Mesh->MeshDataSize);
        
        InitAttribFloat(ModelTranLoc2,
                        4, sizeof(m44),
                        Mesh->MeshDataSize + 1 * sizeof(v4));
        
        InitAttribFloat(ModelTranLoc3,
                        4, sizeof(m44),
                        Mesh->MeshDataSize + 2 * sizeof(v4));
        
        InitAttribFloat(ModelTranLoc4,
                        4, sizeof(m44),
                        Mesh->MeshDataSize + 3 * sizeof(v4));
        
        glVertexAttribDivisor(ModelTranLoc1, 1);
        glVertexAttribDivisor(ModelTranLoc2, 1);
        glVertexAttribDivisor(ModelTranLoc3, 1);
        glVertexAttribDivisor(ModelTranLoc4, 1);
    }
    else
    {
    }
    
    // NOTE(Dima): Uniform skinning matrices
    renderer_handle SkinningMatricesTexBuf = {};
    if(Mesh->IsSkinned)
    {
        OpenGLCreateAndBindTextureBuffer(&SkinningMatricesTexBuf,
                                         sizeof(m44) * NumInstanceSkMat * MeshInstanceCount,
                                         SkinningMatrices,
                                         GL_RGBA32F,
                                         1,
                                         Shader->GetLoc("SkinningMatrices"));
    }
    Shader->SetInt("SkinningMatricesCount", NumInstanceSkMat);
    Shader->SetBool("MeshIsSkinned", Mesh->IsSkinned);
    
    // NOTE(Dima): Setting material
    if (!RenderPass->IsShadowPass)
    {
        b32 MaterialMissing = true;
        
        if(Material != 0)
        {
            MaterialMissing = false;
            
            switch(Material->Type)
            {
                case Material_SpecularDiffuse:
                {
                    image* DiffuseTex = Material->Textures[MatTex_SpecularDiffuse_Diffuse];
                    Shader->SetBool("HasDiffuse", DiffuseTex != 0);
                    
                    if(DiffuseTex != 0)
                    {
                        OpenGL_InitImage(DiffuseTex);
                        
                        Shader->SetTexture2D("TexDiffuse", 
                                             DiffuseTex->Handle.Image.TextureObject,
                                             0);
                    }
                    
                }break;
                
                case Material_PBR:
                {
                    
                }break;
                
                case Material_Solid:
                {
                    
                }break;
            }
        }
        
        Shader->SetBool("MaterialMissing", MaterialMissing);
    }
    
    if(RenderPass->ClippingPlaneIsSet)
    {
        glEnable(GL_CLIP_DISTANCE0);
        Shader->SetVec4("ClippingPlane", RenderPass->ClippingPlane);
    }
    
    // NOTE(Dima): Rendering
    if(UseInstancing)
    {
        glDrawElementsInstanced(GL_TRIANGLES, Mesh->IndexCount, GL_UNSIGNED_INT, 0, 
                                MeshInstanceCount);
    }
    else
    {
        glDrawElements(GL_TRIANGLES, Mesh->IndexCount, GL_UNSIGNED_INT, 0);
    }
    
    OpenGLDeleteHandle(&SkinningMatricesTexBuf);
    
    if(UseInstancing)
    {
        glDisableVertexAttribArray(ModelTranLoc1);
        glDisableVertexAttribArray(ModelTranLoc2);
        glDisableVertexAttribArray(ModelTranLoc3);
        glDisableVertexAttribArray(ModelTranLoc4);
        
        glDeleteBuffers(1, &InstanceModelBO);
    }
    
    if(RenderPass->ClippingPlaneIsSet)
    {
        glDisable(GL_CLIP_DISTANCE0);
    }
}


INTERNAL_FUNCTION void OpenGL_RenderVoxelMesh(render_commands* Commands, 
                                              opengl_shader* Shader,
                                              render_pass* RenderPass,
                                              render_command_voxel_mesh* Command,
                                              image* VoxelAtlas)
{
    FUNCTION_TIMING();
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    voxel_mesh* Mesh = Command->Mesh;
    
#define VOXEL_MESH_ATLAS_TEXTURE_UNIT 0
#define VOXEL_MESH_PER_FACE_TEXTURE_UNIT 1
    
    b32 MeshWasDeleted = OpenGL_ProcessHandleInvalidation(&Mesh->Handle);
    if(!Mesh->Handle.Initialized || MeshWasDeleted)
    {
        if(Mesh->VerticesCount > 0)
        {
            
            // NOTE(Dima): Allocating voxel mesh
            renderer_handle* Handle = &Mesh->Handle;
            
            GLuint VAO = 0;
            GLuint VBO = 0;
            
            // NOTE(Dima): Init VAO and VBO
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            
            glBindVertexArray(VAO);
            
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, 
                         Mesh->VerticesCount * sizeof(u32), 
                         Mesh->Vertices, 
                         GL_STATIC_DRAW);
            
            InitAttribInt(OpenGL->VoxelShader->GetAttribLoc("InPosition"),
                          1, sizeof(u32), 0);
            
            InitAttribInt(OpenGL->VoxelShadowShader->GetAttribLoc("InPosition"),
                          1, sizeof(u32), 0);
            
            glBindVertexArray(0);
            
            InitRendererHandle(Handle, RendererHandle_Mesh);
            Handle->Mesh.ArrayObject = VAO;
            Handle->Mesh.BufferObject = VBO;
            Handle->Initialized = true;
        }
    }
    
    glUseProgram(Shader->ID);
    b32 PerFaceBufWasDeleted = OpenGL_ProcessHandleInvalidation(&Mesh->PerFaceBufHandle);
    if(!Mesh->PerFaceBufHandle.Initialized || PerFaceBufWasDeleted)
    {
        if(Mesh->FaceCount > 0)
        {
            renderer_handle* Handles = &Mesh->PerFaceBufHandle;
            
            // NOTE(Dima): This is not under VAO. I think it's OK
            OpenGLCreateTextureBuffer(Handles,
                                      Mesh->FaceCount * sizeof(u32),
                                      Mesh->PerFaceData,
                                      GL_R32UI, 
                                      VOXEL_MESH_PER_FACE_TEXTURE_UNIT,
                                      true);
        }
    }
    
    glBindVertexArray(Mesh->Handle.Mesh.ArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, Mesh->Handle.Mesh.BufferObject);
    
    // NOTE(Dima): Uniform voxel atlas
    GLuint AtlasTexture = OpenGL_InitImage(VoxelAtlas);
    Shader->SetTexture2D("TextureAtlas", AtlasTexture, VOXEL_MESH_ATLAS_TEXTURE_UNIT);
    Shader->SetMat4("ViewProjection", RenderPass->ViewProjection.e);
    Shader->SetMat4("Projection", RenderPass->Projection.e);
    Shader->SetMat4("View", RenderPass->View.e);
    Shader->SetVec3("ChunkAt", Command->ChunkAt);
    Shader->SetFloat("Time", Commands->Time);
    Shader->SetUIVec3("VL_Shift", 
                      Command->Layout.VL_ShiftX,
                      Command->Layout.VL_ShiftY,
                      Command->Layout.VL_ShiftZ);
    Shader->SetUIVec3("VL_Mask",
                      Command->Layout.VL_MaskX,
                      Command->Layout.VL_MaskY,
                      Command->Layout.VL_MaskZ);
    
    Shader->SetBool("HasClippingPlane", RenderPass->ClippingPlaneIsSet);
    if(RenderPass->ClippingPlaneIsSet)
    {
        glEnable(GL_CLIP_DISTANCE0);
        Shader->SetVec4("ClippingPlane", RenderPass->ClippingPlane);
    }
    
    // NOTE(Dima): Uniform per-face data
    OpenGLBindTextureBuffer(&Mesh->PerFaceBufHandle,
                            VOXEL_MESH_PER_FACE_TEXTURE_UNIT,
                            Shader->GetLoc("PerFaceData"));
    
    glDrawArrays(GL_TRIANGLES, 0, Mesh->VerticesCount);
    
    if(RenderPass->ClippingPlaneIsSet)
    {
        glDisable(GL_CLIP_DISTANCE0);
    }
}

INTERNAL_FUNCTION void OpenGL_RenderImage(render_commands* Commands,
                                          render_command_image* ImageCommand)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->UIRectShader;
    
    image* Image = ImageCommand->Image;
    v2 P = ImageCommand->P;
    f32 Width = ImageCommand->Dim.x;
    f32 Height = ImageCommand->Dim.y;
    v4 C = ImageCommand->C;
    b32 DisableDepthTest = ImageCommand->DisableDepthTest;
    
    if(DisableDepthTest)
    {
        glDisable(GL_DEPTH_TEST);
    }
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    // NOTE(Dima): Init quad
    GLfloat QuadData[] = {
        P.x, P.y, 0.0f, 0.0f,
        P.x + Width, P.y, 1.0f, 0.0f, 
        P.x + Width, P.y + Height, 1.0f, 1.0f,
        P.x, P.y + Height, 0.0f, 1.0f,
    };
    
    GLuint QuadIndices[] = 
    {
        0, 1, 2,
        0, 2, 3,
    };
    
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadData), QuadData, GL_STREAM_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QuadIndices), QuadIndices, GL_STREAM_DRAW);
    
    InitAttribFloat(Shader->GetAttribLoc("InPosUV"), 4, 4 * sizeof(float), 0);
    
    // NOTE(Dima): Using program and setting uniforms
    Shader->Use();
    Shader->SetMat4("ViewProjection", OrthographicProjectionWindow(Commands->WindowDimensions.Current.Width,
                                                                   Commands->WindowDimensions.Current.Height));
    Shader->SetVec4("MultColor", C.r, C.g, C.b, C.a);
    Shader->SetBool("IsBatch", false);
    
    OpenGL_InitImage(Image);
    if(Image)
    {
        
        Shader->SetTexture2D("Samplers[0]", Image->Handle.Image.TextureObject, 0);
    }
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // NOTE(Dima): Free everything we need
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    if(DisableDepthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }
    
    glDisable(GL_BLEND);
}

INTERNAL_FUNCTION void OpenGL_ProcessClearCommand(render_commands* Commands)
{
    // NOTE(Dima): Process clear color
    u32 OpenGLFlags = 0;
    v3 ClearC = V3(0.0f, 0.5f, 0.9f);
    
    if(Commands->ClearCommand.Set)
    {
        u32 OurFlags = Commands->ClearCommand.Flags;
        if(OurFlags & RenderClear_Color)
        {
            OpenGLFlags |= GL_COLOR_BUFFER_BIT;
        }
        
        if(OurFlags & RenderClear_Depth)
        {
            OpenGLFlags |= GL_DEPTH_BUFFER_BIT;
        }
        
        if(OurFlags & RenderClear_Stencil)
        {
            OpenGLFlags |= GL_STENCIL_BUFFER_BIT;
        }
        
        ClearC = Commands->ClearCommand.C;
    }
    else
    {
        OpenGLFlags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    }
    
    glClearColor(ClearC.r,
                 ClearC.g,
                 ClearC.b,
                 1.0f);
    glClear(OpenGLFlags);
}

INTERNAL_FUNCTION void OpenGL_DrawSky(render_commands* Commands, 
                                      render_pass* Pass)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->SkyShader;
    
    if(Commands->Sky)
    {
        Shader->Use();
        
        m44 View = Pass->View;
        View.Rows[3] = V4(0.0f, 0.0f, 0.0f, 1.0f);
        Shader->SetMat4("View", View.e);
        Shader->SetMat4("Projection", Pass->Projection.e);
        Shader->SetInt("SkyType", Commands->SkyType);
        Shader->SetVec3("SkyColor", Commands->SkyColor);
        
        if(Commands->Sky)
        {
            OpenGLInitCubemap(Commands->Sky);
            glBindTexture(GL_TEXTURE_CUBE_MAP, Commands->Sky->Handle.Cubemap.Handle);
        }
        
        glBindVertexArray(OpenGL->SkyboxCube.VAO);
        glDepthMask(GL_FALSE);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
    }
}

INTERNAL_FUNCTION void OpenGL_RenderRectBuffer(render_commands* Commands, 
                                               batch_rect_buffer* RectBuffer,
                                               b32 DisableDepthTest = true)
{
    // NOTE(Dima): Skip if we have no rectangles to render
    if (RectBuffer->RectCount == 0)
    {
        return;
    }
    
    if(DisableDepthTest)
    {
        glDisable(GL_DEPTH_TEST);
    }
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->UIRectShader;
    
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(rect_vertex) * RectBuffer->RectCount * 4,
                 &RectBuffer->Vertices[0], 
                 GL_STREAM_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 sizeof(u32) * RectBuffer->RectCount * 6, 
                 &RectBuffer->Indices[0], 
                 GL_STREAM_DRAW);
    
    InitAttribFloat(Shader->GetAttribLoc("InPosUV"), 4, 4 * sizeof(float), 0);
    
    Shader->Use();
    Shader->SetMat4("ViewProjection", RectBuffer->ViewProjection);
    Shader->SetVec4("MultColor", 1.0f, 1.0f, 1.0f, 1.0f);
    Shader->SetBool("IsBatch", true);
    
    for (int i = 0; i < RectBuffer->TextureCount; i++)
    {
        char Buf[32];
        stbsp_sprintf(Buf, "Samplers[%d]", i);
        
        image* Img = RectBuffer->TextureAtlases[i];
        
        OpenGL_InitImage(Img);
        if (Img)
        {
            Shader->SetTexture2D(Buf, Img->Handle.Image.TextureObject, 2 + i);
        }
    }
    
    // NOTE(Dima): Creating and binding colors buffer
    renderer_handle ColorsTexBuf = {};
    OpenGLCreateAndBindTextureBuffer(&ColorsTexBuf,
                                     sizeof(u32) * RectBuffer->RectCount,
                                     &RectBuffer->Colors[0],
                                     GL_R32UI,
                                     0,
                                     Shader->GetLoc("RectsColors"));
    
    // NOTE(Dima): Creating and binding geometry TextureIndices buffer
    renderer_handle TextureIndicesTexBuf = {};
    OpenGLCreateAndBindTextureBuffer(&TextureIndicesTexBuf,
                                     sizeof(u8) * RectBuffer->RectCount,
                                     &RectBuffer->TextureIndices[0],
                                     GL_R8UI, 1, 
                                     Shader->GetLoc("RectsTextureIndices"));
    
    
    glDrawElements(GL_TRIANGLES, RectBuffer->RectCount * 6, GL_UNSIGNED_INT, 0);
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    OpenGLDeleteHandle(&ColorsTexBuf);
    OpenGLDeleteHandle(&TextureIndicesTexBuf);
    
    glUseProgram(0);
    
    
    glDisable(GL_BLEND);
    
    if(DisableDepthTest)
    {
        glEnable(GL_DEPTH_TEST);
    }
}

INTERNAL_FUNCTION void OpenGL_RenderCommands(render_commands* Commands, render_pass* RenderPass)
{
    FUNCTION_TIMING();
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    opengl_shader* StdShader = OpenGL->StdShader;
    opengl_shader* VoxelShader = OpenGL->VoxelShader;
    
    if(RenderPass->IsShadowPass)
    {
        StdShader = OpenGL->StdShadowShader;
        VoxelShader = OpenGL->VoxelShadowShader;
    }
    else
    {
        OpenGL_ProcessClearCommand(Commands);
    }
    
    // NOTE(Dima): Preinit temp voxel buffer
    int VoxelVertsCount = 0;
    int VoxelFaceCount = 0;
    int VoxelChunkCount = 0;
    
    // NOTE(Dima): Looping through all commands
    for(int CommandIndex = 0;
        CommandIndex < Commands->CommandCount;
        CommandIndex++)
    {
        render_command_header* Header = &Commands->CommandHeaders[CommandIndex];
        
        switch(Header->CommandType)
        {
            case RenderCommand_Image:
            {
                render_command_image* ImageCommand = GetRenderCommand(Commands, 
                                                                      CommandIndex,
                                                                      render_command_image);
                
                OpenGL_RenderImage(Commands,
                                   ImageCommand);
            }break;
            
            case RenderCommand_RectBuffer:
            {
                render_command_rect_buffer* RectBuf = GetRenderCommand(Commands,
                                                                       CommandIndex,
                                                                       render_command_rect_buffer);
                
                OpenGL_RenderRectBuffer(Commands, 
                                        RectBuf->RectBuffer,
                                        RectBuf->DisableDepthTest);
            }break;
            
            case RenderCommand_Mesh:
            {
                render_command_mesh* MeshCommand = GetRenderCommand(Commands, 
                                                                    CommandIndex, 
                                                                    render_command_mesh);
                
                OpenGL_RenderMesh(Commands,
                                  StdShader,
                                  RenderPass,
                                  MeshCommand->Mesh,
                                  MeshCommand->Material,
                                  MeshCommand->C,
                                  MeshCommand->SkinningMatrices,
                                  MeshCommand->SkinningMatricesCount,
                                  &MeshCommand->ModelToWorld, 1,
                                  false);
            }break;
            
            case RenderCommand_InstancedMesh:
            {
                render_command_instanced_mesh* MeshCommand = GetRenderCommand(Commands, 
                                                                              CommandIndex,
                                                                              render_command_instanced_mesh);
                
                OpenGL_RenderMesh(Commands,
                                  StdShader,
                                  RenderPass,
                                  MeshCommand->Mesh,
                                  MeshCommand->Material,
                                  MeshCommand->C,
                                  MeshCommand->InstanceSkinningMatrices,
                                  MeshCommand->NumSkinningMatricesPerInstance,
                                  MeshCommand->InstanceMatrices, 
                                  MeshCommand->InstanceCount,
                                  true);
            }break;
            
            case RenderCommand_VoxelChunkMesh:
            {
                render_command_voxel_mesh* MeshCommand = GetRenderCommand(Commands, 
                                                                          CommandIndex,
                                                                          render_command_voxel_mesh);
                
                voxel_mesh* Mesh = MeshCommand->Mesh;
                
                
                b32 IsCulled = IsFrustumCulled(RenderPass, 
                                               &MeshCommand->CullingInfo, 
                                               Commands->CullingEnabled);
                
                if(!IsCulled)
                {
                    OpenGL_RenderVoxelMesh(Commands,
                                           VoxelShader,
                                           RenderPass,
                                           MeshCommand,
                                           Commands->VoxelAtlas);
                }
            }break;
        }
    }
    
    if(RenderPass->IsShadowPass)
    {
        
    }
    else
    {
        // NOTE(Dima): Drawing sky
        OpenGL_DrawSky(Commands, RenderPass);
    }
}

INTERNAL_FUNCTION
void OpenGL_DoShadowMapBlurPass(render_commands* Commands,
                                int InTextureIndex,
                                b32 IsHorizontalPass)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    lighting* Lighting = &Commands->Lighting;
    opengl_framebuffer* ShadowMapsFB = &OpenGL->ShadowMapsFramebuffer;
    post_processing* PP = &Commands->PostProcessing;
    
    int BlurRadius = Lighting->VarianceShadowMapBlurRadius;
    
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    
    opengl_shader* BlurShader = OpenGL->VarianceShadowBlurShader;
    BlurShader->Use();
    BlurShader->SetTexture2DArray("TextureArray", 
                                  ShadowMapsFB->ColorTextures[0], 0);
    BlurShader->SetInt("TextureIndex", InTextureIndex);
    BlurShader->SetBool("IsHorizontalPass", IsHorizontalPass);
    BlurShader->SetInt("BlurRadius", BlurRadius);
    
    Assert(BlurRadius < ArrLen(PP->Gaussian1DKernelForRadius));
    BlurShader->SetFloatArray("GaussianKernel", 
                              PP->Gaussian1DKernelForRadius[BlurRadius], 
                              BlurRadius + 1);
    
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

INTERNAL_FUNCTION 
void OpenGL_RenderShadowMaps(render_commands* Commands)
{
    FUNCTION_TIMING();
    
    // NOTE(Dima): DEPTH TEST SHOULD BE ENABLED
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    lighting* Lighting = &Commands->Lighting;
    opengl_framebuffer* ShadowMapsFB = &OpenGL->ShadowMapsFramebuffer;
    
    if(Lighting->DirLit.CalculateShadows)
    {
        glEnable(GL_DEPTH_TEST);
        
        glBindFramebuffer(GL_FRAMEBUFFER, ShadowMapsFB->Framebuffer);
        glViewport(0, 0,
                   ShadowMapsFB->Resolution.Width,
                   ShadowMapsFB->Resolution.Height);
        
        for(int CascadeIndex = 0;
            CascadeIndex < OpenGL->InitCascadesCount;
            CascadeIndex++)
        {
            shadow_cascade_info* Cascade = &Commands->Lighting.Cascades[CascadeIndex];
            render_pass* ShadowPass = Cascade->RenderPass;
            
            // NOTE(Dima): Now bind layer to copy and create depth squared buffer
            glFramebufferTextureLayer(GL_FRAMEBUFFER,
                                      GL_COLOR_ATTACHMENT0,
                                      ShadowMapsFB->ColorTextures[0],
                                      0, CascadeIndex);
            
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            
            // NOTE(Dima): Rendering
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            OpenGL_RenderCommands(Commands, ShadowPass);
            
            if(Lighting->BlurVarianceShadowMaps)
            {
                glDisable(GL_DEPTH_TEST);
                
                // NOTE(Dima): Apply blur horizontal
                glFramebufferTextureLayer(GL_FRAMEBUFFER,
                                          GL_COLOR_ATTACHMENT0,
                                          ShadowMapsFB->ColorTextures[0],
                                          0, OpenGL->InitCascadesCount);
                OpenGL_DoShadowMapBlurPass(Commands, 
                                           CascadeIndex, 
                                           true);
                
                // NOTE(Dima): Apply blur vertical
                glFramebufferTextureLayer(GL_FRAMEBUFFER,
                                          GL_COLOR_ATTACHMENT0,
                                          ShadowMapsFB->ColorTextures[0],
                                          0, CascadeIndex);
                OpenGL_DoShadowMapBlurPass(Commands, 
                                           OpenGL->InitCascadesCount,
                                           false);
                
                glEnable(GL_DEPTH_TEST);
            }
        }
    }
}

INTERNAL_FUNCTION opengl_framebuffer* OpenGL_DoLightingPass(render_commands* Commands,
                                                            render_pass* RenderPass,
                                                            framebuffer_in_pool_params InPoolParams,
                                                            u32 SSAOTexture,
                                                            char* PostProcSSAO_GUID)
{
    FUNCTION_TIMING();
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    lighting* Lighting = &Commands->Lighting;
    post_processing* PP = &Commands->PostProcessing;
    opengl_framebuffer* GBuf = &OpenGL->GBuffer;
    
    opengl_framebuffer* Result = OpenGL_GetPoolFramebuffer(Commands, InPoolParams);
    
    // NOTE(Dima): LIGHTING PASS. Preparing GBuffer
    opengl_shader* LitShader = OpenGL->LightingShader;
    LitShader->Use();
    
    const m44& Projection = RenderPass->Projection;
    LitShader->SetVec4("PerspProjCoefs",
                       Projection.e[0],
                       Projection.e[5],
                       Projection.e[10],
                       Projection.e[14]);
    
    LitShader->SetVec2("WH", 
                       RenderPass->Width,
                       RenderPass->Height);
    
    LitShader->SetTexture2D("ColorSpecTex",
                            GBuf->ColorTextures[GBufferTex_Colors], 
                            0);
    
    LitShader->SetTexture2D("NormalTex",
                            GBuf->ColorTextures[GBufferTex_Normals], 
                            1);
    
    LitShader->SetTexture2D("PositionsTex",
                            GBuf->ColorTextures[GBufferTex_Positions],
                            2);
    
    LitShader->SetTexture2D("DepthTex",
                            GBuf->DepthTexture, 3);
    
    b32 SSAOEnabled = PostProcEffect_IsEnabled(PP, PostProcSSAO_GUID);
    LitShader->SetBool("SSAOEnabled", SSAOEnabled);
    if(SSAOEnabled)
    {
        ssao_params* SSAOParams = PostProcEffect_GetParams(PP, PostProcSSAO_GUID, ssao_params);
        
        LitShader->Use();
        LitShader->SetFloat("SSAOContribution", SSAOParams->Contribution);
        LitShader->SetTexture2D("SSAOTex", SSAOTexture, 4);
    }
    
    // NOTE(Dima): Uniform lighting variables
    LitShader->SetVec3("CameraP", RenderPass->CameraP);
    LitShader->SetFloat("AmbientPercentage", Lighting->AmbientPercentage);
    LitShader->SetFloat("ShadowStrength", Lighting->ShadowStrength);
    
    LitShader->SetVec3("DirectionalLightDirection", Lighting->DirLit.Dir);
    LitShader->SetVec3("DirectionalLightColor", Lighting->DirLit.C);
    LitShader->SetBool("CalculateDirLightShadow", Lighting->DirLit.CalculateShadows);
    LitShader->SetTexture2DArray("LightDepthTex", 
                                 OpenGL->ShadowMapsFramebuffer.ColorTextures[0], 5);
    
    f32 Distances[8];
    m44 LitProjections[8];
    for(int CascadeIndex = 0;
        CascadeIndex < Lighting->CascadeCount;
        CascadeIndex++)
    {
        shadow_cascade_info* Cascade = &Lighting->Cascades[CascadeIndex];
        
        Distances[CascadeIndex] = Cascade->SourceFarPlane;
        LitProjections[CascadeIndex] = Cascade->RenderPass->ViewProjection;
    }
    
    LitShader->SetFloatArray("CascadeDistances", Distances, Lighting->CascadeCount);
    LitShader->SetMat4Array("CascadeLightProjections", LitProjections, Lighting->CascadeCount);
    LitShader->SetInt("CascadeCount", Lighting->CascadeCount);
    
    LitShader->SetInt("TypeColorOutput", Commands->TypeColorOutput);
    
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    return(Result);
}

INTERNAL_FUNCTION void OpenGL_RenderPassToGBuffer(render_commands* Commands,
                                                  render_pass* Pass)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    // NOTE(Dima): Rendering to GBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, OpenGL->GBuffer.Framebuffer);
    glViewport(0, 0,
               OpenGL->GBuffer.Resolution.Width,
               OpenGL->GBuffer.Resolution.Height);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    OpenGL_RenderCommands(Commands, Pass);
}

INTERNAL_FUNCTION 
opengl_framebuffer* OpenGL_DeferredRender(render_commands* Commands,
                                          render_pass* Pass,
                                          rt_downscale_res Resolution,
                                          char* PostProcSSAO_GUID)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    post_processing* PP = &Commands->PostProcessing;
    opengl_framebuffer* GBuf = &OpenGL->GBuffer;
    
    // NOTE(Dima): Rendering to GBuffer
    OpenGL_RenderPassToGBuffer(Commands, Pass);
    
    // NOTE(Dima): Making SSAO Here not to mess with shader rebinding
    u32 SSAOTexture = 0;
    if (PostProcEffect_IsEnabled(PP, PostProcSSAO_GUID))
    {
        SSAOTexture = OpenGL_DoSSAO(Commands, 
                                    Pass,
                                    GBuf->ColorTextures[GBufferTex_Normals],
                                    GBuf->DepthTexture,
                                    PostProcSSAO_GUID);
    }
    
    
    framebuffer_in_pool_params InPoolParams = {};
    InPoolParams.Resolution = Resolution;
    InPoolParams.Type = FramebufPoolType_Color;
    opengl_framebuffer* LightingPass = OpenGL_DoLightingPass(Commands, 
                                                             Pass, 
                                                             InPoolParams,
                                                             SSAOTexture,
                                                             PostProcSSAO_GUID);
    
    // NOTE(Dima): Freeing SSAO Framebuffer
    return(LightingPass);
}

INTERNAL_FUNCTION opengl_framebuffer* OpenGL_RenderWater(render_commands* Commands,
                                                         opengl_framebuffer* Scene,
                                                         opengl_framebuffer* WaterReflection,
                                                         u32 PositionsTex,
                                                         u32 DepthTex,
                                                         render_pass* MainRenderPass)
{
    framebuffer_in_pool_params InPoolParams = {};
    InPoolParams.Resolution = DownscaleRes_1;
    InPoolParams.Type = FramebufPoolType_Color;
    opengl_framebuffer* Result = OpenGL_GetPoolFramebuffer(Commands, 
                                                           InPoolParams);
    
    render_water* Water = &Commands->Water;
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->RenderWaterShader;
    
    // NOTE(Dima): Setting shader uniforms
    Shader->Use();
    
    Shader->SetTexture2D("SceneRender", Scene->ColorTextures[0], 0);
    Shader->SetTexture2D("ScenePositionsTex", PositionsTex, 1);
    Shader->SetTexture2D("ReflectionRender", WaterReflection->ColorTextures[0], 2);
    Shader->SetTexture2D("SceneDepthTex", DepthTex, 3);
    
    Shader->SetVec4("ClipPlane", Water->PlaneEquation);
    Shader->SetVec3("CameraP", MainRenderPass->CameraP);
    
    Shader->SetVec3("WaterColor", Water->Params.Color.rgb);
    
    // NOTE(Dima): Render quad
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    return(Result);
}

INTERNAL_FUNCTION void OpenGL_RenderDepthRect(render_commands* Commands,
                                              u32 DepthTexture,
                                              int TextureIndex)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->RenderDepthShader;
    
    b32 IsSecondDepth = (TextureIndex & 1) != 0;
    int CascadeIndex = TextureIndex / 2;
    
    f32 OneTextureHeight = MinFloat(2.0f / (f32)OpenGL->InitCascadesCount, 0.75f);
    
    f32 StartX = -1.0f;
    f32 StartY = -1.0f + OneTextureHeight * (f32)CascadeIndex;
    
    if(IsSecondDepth)
    {
        StartX = -0.5f;
    }
    
    opengl_array_object Quad = OpenGL_BeginScreenQuad(V2(StartX, 
                                                         StartY), 
                                                      V2(0.5), 
                                                      GL_STREAM_DRAW);
    
    Shader->Use();
    Shader->SetTexture2DArray("DepthTextureArray", DepthTexture, 0);
    Shader->SetInt("DepthLayerIndex", CascadeIndex);
    Shader->SetBool("SamplerIsArrayTexture", true);
    Shader->SetBool("IsSecondDepth", IsSecondDepth);
    
    glBindVertexArray(Quad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    OpenGL_DeleteArrayObject(&Quad);
}

INTERNAL_FUNCTION void OpenGL_RenderDepthRects(render_commands* Commands)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    glDisable(GL_DEPTH_TEST);
    
    for(int TextureIndex = 0;
        TextureIndex < OpenGL->InitCascadesCount * 2;
        TextureIndex++)
    {
        OpenGL_RenderDepthRect(Commands, 
                               OpenGL->ShadowMapsFramebuffer.ColorTextures[0],
                               TextureIndex);
    }
    
    glEnable(GL_DEPTH_TEST);
}

INTERNAL_FUNCTION PLATFORM_RENDERER_BEGIN_FRAME(OpenGLBeginFrame)
{
    
}

INTERNAL_FUNCTION void OpenGLProcessDeallocList(render_commands* Commands)
{
    render_api_dealloc_entry* At = Commands->UseDealloc.Next;
    
    while(At != &Commands->UseDealloc)
    {
        OpenGLDeleteHandle(At->Handle);
        
        At = At->Next;
    }
    
    BeginTicketMutex(&Commands->DeallocEntriesMutex);
    DLIST_REMOVE_ENTIRE_LIST(&Commands->UseDealloc, &Commands->FreeDealloc, Next, Prev);
    EndTicketMutex(&Commands->DeallocEntriesMutex);
}

INTERNAL_FUNCTION void OpenGL_RenderPathDeferred(render_commands* Commands)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    post_processing* PP = &Commands->PostProcessing;
    render_pass* Pass = &Commands->RenderPasses[0];
    render_pass* WaterPass = Commands->Water.ReflectionPass;
    
    // NOTE(Dima): Rendering to shadow buffer
    OpenGL_RenderShadowMaps(Commands);
    
    opengl_framebuffer* WaterRenderResult = 0;
    opengl_framebuffer* CombineRenderResult = 0;
    
    // NOTE(Dima): Combining water and scene render pass results
    if(Commands->WaterIsSet)
    {
        WaterRenderResult = OpenGL_DeferredRender(Commands, WaterPass, 
                                                  DownscaleRes_1,
                                                  "InWaterSSAO");
        
        // NOTE(Dima): Doing scene pass after water pass so that we have fresh positions texture
        // NOTE(Dima): that we need in water rendering next
        opengl_framebuffer* SceneRenderResult = OpenGL_DeferredRender(Commands, Pass,
                                                                      DownscaleRes_1,
                                                                      "MainSSAO");
        
        CombineRenderResult = OpenGL_RenderWater(Commands,
                                                 SceneRenderResult,
                                                 WaterRenderResult,
                                                 OpenGL->GBuffer.ColorTextures[GBufferTex_Positions],
                                                 OpenGL->GBuffer.DepthTexture,
                                                 Pass);
        
        OpenGL_ReleasePoolFramebuffer(Commands, SceneRenderResult);
        OpenGL_ReleasePoolFramebuffer(Commands, WaterRenderResult);
    }
    else
    {
        CombineRenderResult = OpenGL_DeferredRender(Commands, Pass,
                                                    DownscaleRes_1,
                                                    "MainSSAO");
    }
    
    if (PostProcEffect_IsEnabled(PP, "MainDOF") && PostProcEffect_IsEnabled(PP, "MainDOF_Blur"))
    {
        // NOTE(Dima): Some post processing
        opengl_framebuffer* LittleBlur = OpenGL_DoBoxBlur(Commands, 
                                                          CombineRenderResult->ColorTextures[0], 
                                                          "MainDOF_Blur");
        
        opengl_framebuffer* DepthOfField = OpenGL_DoDepthOfField(Commands,
                                                                 CombineRenderResult->ColorTextures[0],
                                                                 LittleBlur->ColorTextures[0],
                                                                 OpenGL->GBuffer.DepthTexture,
                                                                 "MainDOF");
        
        OpenGL_ReleasePoolFramebuffer(Commands, LittleBlur);
        OpenGL_ReleasePoolFramebuffer(Commands, CombineRenderResult);
        
        
        // TODO(Dima): Fit rect to center of target framebuffer
        // NOTE(Dima): Copying to main framebuffer
        OpenGL_CopyColorFromToFramebuffer(DepthOfField->Framebuffer, 
                                          DepthOfField->Resolution, 
                                          0, Commands->WindowDimensions.Current);
        
        OpenGL_ReleasePoolFramebuffer(Commands, DepthOfField);
    }
    else
    {
        // TODO(Dima): Fit rect to center of target framebuffer
        // NOTE(Dima): Copying to main framebuffer
        int Width = Commands->WindowDimensions.Current.Width;
        int Height = Commands->WindowDimensions.Current.Height;
        
        OpenGL_CopyColorFromToFramebuffer(CombineRenderResult->Framebuffer, 
                                          CombineRenderResult->Resolution,
                                          0, Commands->WindowDimensions.Current);
        
        OpenGL_ReleasePoolFramebuffer(Commands, CombineRenderResult);
    }
}

INTERNAL_FUNCTION void OpenGL_RenderPathForward(render_commands* Commands)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    render_pass* Pass = &Commands->RenderPasses[0];
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    iv2 CurDim = Commands->WindowDimensions.Current;
    
    glViewport(0, 0, CurDim.Width, CurDim.Height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    OpenGL_RenderCommands(Commands, Pass);
    
    // NOTE(Dima): Getting framebuffer
    framebuffer_in_pool_params InPoolParams = {};
    InPoolParams.Resolution = DownscaleRes_1;
    InPoolParams.Type = FramebufPoolType_Color;
    opengl_framebuffer* MainRender = OpenGL_GetPoolFramebuffer(Commands, InPoolParams);
    
    OpenGL_CopyColorFromToFramebuffer(0, CurDim,
                                      MainRender->Framebuffer, 
                                      MainRender->Resolution);
    
#if 0
    opengl_framebuffer* Final = OpenGL_DoCrtDisplay(Commands,
                                                    MainRender->ColorTextures[0],
                                                    "CrtDisplay");
#else
    opengl_framebuffer* Final = OpenGL_DoPosterize(Commands,
                                                   MainRender->ColorTextures[0],
                                                   "Posterize");
#endif
    
    OpenGL_ReleasePoolFramebuffer(Commands, MainRender);
    
    
    // NOTE(Dima): Copying to main framebuffer
    OpenGL_CopyColorFromToFramebuffer(Final->Framebuffer, Final->Resolution, 
                                      0, CurDim);
    
    OpenGL_ReleasePoolFramebuffer(Commands, Final);
}

INTERNAL_FUNCTION PLATFORM_RENDERER_RENDER(OpenGL_Render)
{
    FUNCTION_TIMING();
    
    OpenGLProcessDeallocList(Commands);
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    if (Commands->Lighting.NeedResettingShadowMaps)
    {
        OpenGL_FreeShadowMaps(Commands);
        OpenGL_InitShadowMaps(Commands);
        
        Commands->Lighting.NeedResettingShadowMaps = false;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    //if (Commands->IsDeferredRenderer)
    if (true)
    {
        OpenGL_RenderPathDeferred(Commands);
    }
    else
    {
        OpenGL_RenderPathForward(Commands);
    }
    
    // NOTE(Dima): Rendering DEBUG rect buffers
    
    //OpenGL_RenderDepthRects(Commands);
    OpenGL_RenderRectBuffer(Commands, Commands->DEBUG_Rects2D_Unit);
    OpenGL_RenderRectBuffer(Commands, Commands->DEBUG_Rects2D_Window);
    
    OpenGL_PoolsAreEmptyCheck(Commands);
}

INTERNAL_FUNCTION PLATFORM_RENDERER_PRESENT(OpenGL_Present)
{
    SDL_GL_SwapWindow(App->Window);
}