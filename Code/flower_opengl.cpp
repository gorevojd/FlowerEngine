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

INTERNAL_FUNCTION GLuint OpenGLLoadProgram(char* VertexFilePath, 
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

INTERNAL_FUNCTION opengl_shader* OpenGLLoadShader(opengl_state* OpenGL,
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
    
    
    Result->ID = OpenGLLoadProgram(VertexFilePath, 
                                   FragmentFilePath,
                                   GeometryFilePath);
    
    if(Result->ID == -1)
    {
        SDL_Log("%s shader loaded successfully!\n", ShaderName);
    }
    
    CopyStringsSafe(Result->Name, ArrayCount(Result->Name), ShaderName);
    
    // NOTE(Dima): Adding this shader to LoadedShaders
    opengl_loaded_shader* LoadedShader = PushStruct(OpenGL->Arena, opengl_loaded_shader);
    LoadedShader->Shader = Result;
    LoadedShader->NextInLoadedShaderList = OpenGL->LoadedShadersList;
    OpenGL->LoadedShadersList = LoadedShader;
    
    return(Result);
}

INTERNAL_FUNCTION void OpenGLDeleteShader(opengl_shader* Shader)
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

INTERNAL_FUNCTION b32 OpenGLProcessHandleInvalidation(renderer_handle* Handle)
{
    b32 WasDeleted = ShouldDeleteHandleStorage(Handle);
    
    if(WasDeleted)
    {
        OpenGLDeleteHandle(Handle);
        
        Handle->Invalidated = false;
    }
    
    return(WasDeleted);
}

INTERNAL_FUNCTION GLuint OpenGLInitImage(image* Image)
{
    GLuint TexOpenGL = 0;
    
    b32 ImageWasDeleted = OpenGLProcessHandleInvalidation(&Image->Handle);
    
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
    
    b32 WasDeleted = OpenGLProcessHandleInvalidation(CubemapHandle);
    
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


INTERNAL_FUNCTION void OpenGL_BindPP(opengl_pp_framebuffer* PPFB)
{
    glBindFramebuffer(GL_FRAMEBUFFER, PPFB->FB.Framebuffer);
    
    glViewport(0, 0, 
               PPFB->FB.Width,
               PPFB->FB.Height);
}

INTERNAL_FUNCTION opengl_pp_framebuffer* OpenGL_BeginPP(render_commands* Commands,
                                                        pp_resolution Resolution)
{
    FUNCTION_TIMING();
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    opengl_pp_framebuffer_pool* Pool = 0;
    
    switch(Resolution)
    {
        case PostProcessResolution_Normal:
        {
            Pool = &OpenGL->FramebufPoolNormalRes;
        }break;
        
        case PostProcessResolution_Half:
        {
            Pool = &OpenGL->FramebufPoolHalfRes;
        }break;
        
        case PostProcessResolution_Quater:
        {
            Pool = &OpenGL->FramebufPoolQuaterRes;
        }break;
    }
    
    Assert(Pool);
    
    opengl_pp_framebuffer* Found = 0;
    for(int FramebufIndex = 0; FramebufIndex < Pool->Count; FramebufIndex++)
    {
        opengl_pp_framebuffer* PPFB = &Pool->Framebuffers[FramebufIndex];
        
        if(PPFB->IsInUseNow == false)
        {
            Found = PPFB;
            
            Found->IsInUseNow = true;
            
            break;
        }
    }
    
    if(!Found)
    {
        // NOTE(Dima): Have no 
        Assert(!"Have no free framebuffers!!!");
        InvalidCodePath;
    }
    
    return(Found);
}

INTERNAL_FUNCTION void OpenGL_EndPP(opengl_pp_framebuffer* PPFB)
{
    FUNCTION_TIMING();
    
    if(PPFB)
    {
        PPFB->IsInUseNow = false;
    }
}

INTERNAL_FUNCTION opengl_pp_framebuffer* OpenGL_DoDilation(render_commands* Commands, 
                                                           u32 InputTexture,
                                                           pp_dilation_params Params)
{
    FUNCTION_TIMING();
    
    opengl_pp_framebuffer* Result = OpenGL_BeginPP(Commands, Params.Resolution);
    OpenGL_BindPP(Result);
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->DilationShader;
    
    Shader->Use();
    Shader->SetInt("Size", Params.Size);
    Shader->SetVec2("MinMaxThreshold", 
                    Params.MinThreshold, 
                    Params.MaxThreshold);
    Shader->SetTexture2D("InputTexture", 
                         InputTexture, 0);
    
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    return(Result);
}

INTERNAL_FUNCTION opengl_pp_framebuffer* OpenGL_DoPosterize(render_commands* Commands,
                                                            u32 InputTexture,
                                                            int Levels,
                                                            pp_resolution Resolution)
{
    FUNCTION_TIMING();
    
    opengl_pp_framebuffer* Result = OpenGL_BeginPP(Commands, Resolution);
    OpenGL_BindPP(Result);
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->PosterizeShader;
    
    Shader->Use();
    Shader->SetInt("Levels", Levels);
    Shader->SetTexture2D("InputTexture", 
                         InputTexture, 0);
    
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    return(Result);
}

INTERNAL_FUNCTION opengl_pp_framebuffer* OpenGL_DoBoxBlur(render_commands* Commands,
                                                          u32 InputTexture,
                                                          int RadiusSize,
                                                          pp_resolution Resolution,
                                                          b32 EnableCheapMode = true)
{
    FUNCTION_TIMING();
    
    opengl_pp_framebuffer* Result = OpenGL_BeginPP(Commands, Resolution);
    OpenGL_BindPP(Result);
    
    //if (EnableCheapMode)
    {
        // NOTE(Dima): If we use half-res or quater-res textures then we will get blur for free
        //if(Resolution != PostProcessResolution_Normal)
        {
            //RadiusSize = 0.0f;
        }
    }
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->BoxBlurShader;
    
    Shader->Use();
    Shader->SetInt("BlurRadius", RadiusSize);
    Shader->SetTexture2D("ToBlurTex", 
                         InputTexture, 0);
    
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    return(Result);
}

INTERNAL_FUNCTION opengl_pp_framebuffer* OpenGL_DoDepthOfField(render_commands* Commands,
                                                               u32 InputFocus,
                                                               u32 InputOutOfFocus,
                                                               u32 DepthTexture,
                                                               pp_depth_of_field_params Params)
{
    FUNCTION_TIMING();
    
    opengl_pp_framebuffer* Result = OpenGL_BeginPP(Commands, PostProcessResolution_Normal);
    OpenGL_BindPP(Result);
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->DepthOfFieldShader;
    
    const m44& Projection = Commands->RenderPasses[0].Projection;
    
    Shader->Use();
    Shader->SetFloat("MinDistance", Params.MinDistance);
    Shader->SetFloat("MaxDistance", Params.MaxDistance);
    Shader->SetFloat("FocusZ", Params.FocusZ);
    Shader->SetVec2("WH", 
                    Commands->WindowDimensions.Width,
                    Commands->WindowDimensions.Height);
    Shader->SetVec4("PerspProjCoefs",
                    Projection.e[0],
                    Projection.e[5],
                    Projection.e[10],
                    Projection.e[14]);
    
    Shader->SetTexture2D("FocusTex", InputFocus, 0);
    Shader->SetTexture2D("OutOfFocusTex", InputOutOfFocus, 1);
    Shader->SetTexture2D("DepthTex", DepthTexture, 2);
    
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    return(Result);
}

INTERNAL_FUNCTION void OpenGL_SSAO_DoPass(render_commands* Commands, 
                                          opengl_g_buffer* GBuf,
                                          render_pass* RenderPass)
{
    FUNCTION_TIMING();
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_ssao* SSAO = &OpenGL->SSAO;
    opengl_shader* Shader = OpenGL->SSAOShader;
    postprocessing* PP = &Commands->PostProcessing;
    const m44& Projection = RenderPass->Projection;
    
    // NOTE(Dima): Do pass
    glBindFramebuffer(GL_FRAMEBUFFER, SSAO->Framebuffer);
    
    Shader->Use();
    Shader->SetTexture2D("DepthTex", GBuf->Depth, 0);
    Shader->SetTexture2D("NormalTex", GBuf->Normal, 1);
    Shader->SetTexture2D("SSAONoiseTex", SSAO->NoiseTex, 2);
    
    Shader->SetVec3Array("SSAOKernel", 
                         PP->SSAO_Kernel, 
                         PP->SSAO_Params.KernelSize);
    Shader->SetInt("SSAOKernelSamplesCount", PP->SSAO_Params.KernelSize);
    Shader->SetFloat("SSAOKernelRadius", PP->SSAO_Params.KernelRadius);
    Shader->SetFloat("SSAORangeCheck", PP->SSAO_Params.RangeCheck);
    Shader->SetVec2("WH", 
                    Commands->WindowDimensions.Width,
                    Commands->WindowDimensions.Height);
    Shader->SetVec4("PerspProjCoefs",
                    Projection.e[0],
                    Projection.e[5],
                    Projection.e[10],
                    Projection.e[14]);
    Shader->SetMat4("View", RenderPass->View.e);
    
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // NOTE(Dima): SSAO Blur pass
    glBindFramebuffer(GL_FRAMEBUFFER, SSAO->BlurFramebuffer);
    Shader = OpenGL->SSAOBlurShader;
    Shader->Use();
    Shader->SetTexture2D("OcclusionTex", SSAO->FramebufferTexture, 0);
    Shader->SetInt("BlurRadius", PP->SSAO_Params.BlurRadius);
    
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

INTERNAL_FUNCTION void OpenGL_SSAO_Init(render_commands* Commands,
                                        int Width, int Height)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_ssao* SSAO = &OpenGL->SSAO;
    postprocessing* PP = &Commands->PostProcessing;
    
    // NOTE(Dima): init SSAO noise texture
    glGenTextures(1, &SSAO->NoiseTex);
    glBindTexture(GL_TEXTURE_2D, SSAO->NoiseTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &PP->SSAO_Noise);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
    
    // NOTE(Dima): Init SSAO framebuffer
    glGenFramebuffers(1, &SSAO->Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, SSAO->Framebuffer);
    
    glGenTextures(1, &SSAO->FramebufferTexture);
    glBindTexture(GL_TEXTURE_2D, SSAO->FramebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, 
                 GL_RED, 
                 Width, Height, 
                 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, 
                           GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_2D, 
                           SSAO->FramebufferTexture, 0);
    
    OpenGLCheckFramebuffer(__FILE__, __LINE__);
    
    // NOTE(Dima): Init SSAO blur framebuffer
    glGenFramebuffers(1, &SSAO->BlurFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, SSAO->BlurFramebuffer);
    
    glGenTextures(1, &SSAO->BlurFramebufferTexture);
    glBindTexture(GL_TEXTURE_2D, SSAO->BlurFramebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, 
                 GL_RED, 
                 Commands->WindowDimensions.Width, 
                 Commands->WindowDimensions.Height, 
                 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, 
                           GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_2D, 
                           SSAO->BlurFramebufferTexture, 0);
    
    OpenGLCheckFramebuffer(__FILE__, __LINE__);
}

INTERNAL_FUNCTION void OpenGL_SSAO_Free(render_commands* Commands)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_ssao* SSAO = &OpenGL->SSAO;
    
    glDeleteFramebuffers(1, &SSAO->Framebuffer);
    glDeleteFramebuffers(1, &SSAO->BlurFramebuffer);
    
    glDeleteTextures(1, &SSAO->FramebufferTexture);
    glDeleteTextures(1, &SSAO->BlurFramebufferTexture);
    glDeleteTextures(1, &SSAO->NoiseTex);
}

INTERNAL_FUNCTION void OpenGL_GBufferInit(opengl_g_buffer* GBuf, int Width, int Height)
{
    FUNCTION_TIMING();
    
    GBuf->Width = Width;
    GBuf->Height = Height;
    
    // NOTE(Dima): Init framebuffer
    glGenFramebuffers(1, &GBuf->Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, GBuf->Framebuffer);
    
    // NOTE(Dima): Init ColorSpec texture
    glGenTextures(1, &GBuf->ColorSpec);
    glBindTexture(GL_TEXTURE_2D, GBuf->ColorSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GBuf->ColorSpec, 0);
    
    // NOTE(Dima): Init normal texture
    glGenTextures(1, &GBuf->Normal);
    glBindTexture(GL_TEXTURE_2D, GBuf->Normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, GBuf->Normal, 0);
    
    //NOTE(Dima): Init positions texture
    glGenTextures(1, &GBuf->Positions);
    glBindTexture(GL_TEXTURE_2D, GBuf->Positions);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Width, Height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, GBuf->Positions, 0);
    
    u32 Attachments[] = {
        GL_COLOR_ATTACHMENT0, 
        GL_COLOR_ATTACHMENT1, 
        GL_COLOR_ATTACHMENT2, 
    };
    glDrawBuffers(ARC(Attachments), Attachments);
    
    // NOTE(Dima): Init Depth texture
    glGenTextures(1, &GBuf->Depth);
    glBindTexture(GL_TEXTURE_2D, GBuf->Depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, GBuf->Depth, 0);
}

INTERNAL_FUNCTION void OpenGL_GBufferFree(opengl_g_buffer* GBuf)
{
    FUNCTION_TIMING();
    
    glDeleteFramebuffers(1, &GBuf->Framebuffer);
    
    glDeleteTextures(1, &GBuf->ColorSpec);
    glDeleteTextures(1, &GBuf->Normal);
    glDeleteTextures(1, &GBuf->Positions);
    glDeleteTextures(1, &GBuf->Depth);
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
    
    opengl_framebuffer* FB = &OpenGL->ShadowMap;
    FB->Width = Lighting->ShadowMapRes;
    FB->Height = Lighting->ShadowMapRes;
    
    OpenGL->InitCascadesCount = Lighting->CascadeCount;
    
    
    // NOTE(Dima): Generating framebuffer
    glGenFramebuffers(1, &FB->Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, FB->Framebuffer);
    
    
    // NOTE(Dima): Generate depth texture array
    glGenTextures(1, &FB->Texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, FB->Texture);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY,
                   1, 
                   GL_RG32F,
                   Lighting->ShadowMapRes,
                   Lighting->ShadowMapRes,
                   Lighting->CascadeCount + 1); // +1 temp
    
    OpenGLCheckError(__FILE__, __LINE__);
    
    
    // NOTE(Dima): Setting params
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float BorderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, BorderColor);
    
    
    // NOTE(Dima): Creating depth texture
    glGenTextures(1, &FB->DepthTexture);
    glBindTexture(GL_TEXTURE_2D, FB->DepthTexture);
    glTexImage2D(GL_TEXTURE_2D,
                 0, 
                 GL_DEPTH_COMPONENT32F,
                 FB->Width,
                 FB->Height,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D,
                           FB->DepthTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    OpenGLCheckError(__FILE__, __LINE__);
    
    
    // NOTE(Dima): Init texture of random rotations for PCF poisson samples
    glGenTextures(1, &OpenGL->PoissonSamplesRotationTex);
    glBindTexture(GL_TEXTURE_2D, OpenGL->PoissonSamplesRotationTex);
    glTexImage2D(GL_TEXTURE_2D, 0,
                 GL_R32F,
                 4, 4,
                 0, GL_RED,
                 GL_FLOAT, 
                 Lighting->PCF_PoissonSamples.Rotations);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // TODO(Dima): Function to free shadow maps
}

INTERNAL_FUNCTION void OpenGLLoadShaders(render_commands* Commands, memory_arena* Arena)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    // NOTE(Dima): Init shaders
    OpenGL->StdShader = OpenGLLoadShader(OpenGL, "Standard",
                                         "../Data/Shaders/std.vs",
                                         "../Data/Shaders/std.fs");
    
    OpenGL->StdShadowShader = OpenGLLoadShader(OpenGL, "StandardShadow",
                                               "../Data/Shaders/std.vs",
                                               "../Data/Shaders/shadow_depth.fs");
    
    OpenGL->UIRectShader = OpenGLLoadShader(OpenGL, "UIRect",
                                            "../Data/Shaders/ui_rect.vs",
                                            "../Data/Shaders/ui_rect.fs");
    
    OpenGL->VoxelShader = OpenGLLoadShader(OpenGL, "Voxel",
                                           "../Data/Shaders/voxel.vs",
                                           "../Data/Shaders/voxel.fs");
    
    OpenGL->VoxelShadowShader = OpenGLLoadShader(OpenGL, "VoxelShadow",
                                                 "../Data/Shaders/voxel.vs",
                                                 "../Data/Shaders/shadow_depth.fs");
    
    OpenGL->SSAOShader = OpenGLLoadShader(OpenGL, "SSAO",
                                          "../Data/Shaders/screen.vs",
                                          "../Data/Shaders/ssao.fs");
    
    OpenGL->SSAOBlurShader = OpenGLLoadShader(OpenGL, "SSAOBlur",
                                              "../Data/Shaders/screen.vs",
                                              "../Data/Shaders/ssao_blur.fs");
    
    OpenGL->LightingShader = OpenGLLoadShader(OpenGL, "Lighting",
                                              "../Data/Shaders/screen.vs",
                                              "../Data/Shaders/lighting.fs");
    
    OpenGL->BoxBlurShader = OpenGLLoadShader(OpenGL, "BoxBlur",
                                             "../Data/Shaders/screen.vs",
                                             "../Data/Shaders/box_blur.fs");
    
    OpenGL->DilationShader = OpenGLLoadShader(OpenGL, "Dilation",
                                              "../Data/Shaders/screen.vs",
                                              "../Data/Shaders/dilation.fs");
    
    OpenGL->PosterizeShader = OpenGLLoadShader(OpenGL, "Posterize",
                                               "../Data/Shaders/screen.vs",
                                               "../Data/Shaders/posterize.fs");
    
    OpenGL->DepthOfFieldShader = OpenGLLoadShader(OpenGL, "DepthOfField",
                                                  "../Data/Shaders/screen.vs",
                                                  "../Data/Shaders/depth_of_field.fs");
    
    OpenGL->DepthOfFieldShader = OpenGLLoadShader(OpenGL, "DepthOfField",
                                                  "../Data/Shaders/screen.vs",
                                                  "../Data/Shaders/depth_of_field.fs");
    
    OpenGL->SkyShader = OpenGLLoadShader(OpenGL, "Sky",
                                         "../Data/Shaders/sky.vs",
                                         "../Data/Shaders/sky.fs");
    
    OpenGL->RenderDepthShader = OpenGLLoadShader(OpenGL, "RenderDepth",
                                                 "../Data/Shaders/screen.vs",
                                                 "../Data/Shaders/render_depth.fs");
    
    OpenGL->VarianceShadowBlurShader = OpenGLLoadShader(OpenGL, "VarianceShadowmapBlur",
                                                        "../Data/Shaders/screen.vs",
                                                        "../Data/Shaders/variance_shadowmap_blur.fs");
    
    OpenGL->RenderWaterShader = OpenGLLoadShader(OpenGL, "RenderWater",
                                                 "../Data/Shaders/screen.vs",
                                                 "../Data/Shaders/render_water.fs");
}

INTERNAL_FUNCTION opengl_pp_framebuffer_pool OpenGL_InitFramebufferPool(render_commands* Commands,
                                                                        int NumFramebuffers,
                                                                        int Width, int Height,
                                                                        pp_resolution Resolution)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    // NOTE(Dima): Init the result
    opengl_pp_framebuffer_pool Result = {};
    
    Result.Framebuffers = PushArray(Commands->Arena, 
                                    opengl_pp_framebuffer, 
                                    NumFramebuffers);
    
    Result.Count = NumFramebuffers;
    
    // NOTE(Dima): Init post process framebuffer pool
    for(int Index = 0;
        Index < NumFramebuffers;
        Index++)
    {
        opengl_pp_framebuffer* PPFB = Result.Framebuffers + Index;
        
        PPFB->IsInUseNow = false;
        
        // NOTE(Dima): Init actual framebuffer
        opengl_framebuffer* FB = &PPFB->FB;
        
        FB->Width = Width;
        FB->Height = Height;
        
        // NOTE(Dima): Generating framebuffer
        glGenFramebuffers(1, &FB->Framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, FB->Framebuffer);
        
        OpenGLCheckError(__FILE__, __LINE__);
        
        GLuint FilterMode = GL_NEAREST;
        if(Resolution != PostProcessResolution_Normal)
        {
            // NOTE(Dima): Allow linear filtering to get cheap blur for downscaled framebuffers
            FilterMode = GL_LINEAR;
        }
        
        // NOTE(Dima): Generating texture attachment
        glGenTextures(1, &FB->Texture);
        glBindTexture(GL_TEXTURE_2D, FB->Texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, 
                     GL_RGB8, 
                     Width, Height, 
                     0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, 
                               GL_COLOR_ATTACHMENT0, 
                               GL_TEXTURE_2D, 
                               FB->Texture, 0);
        
        OpenGLCheckError(__FILE__, __LINE__);
        
        OpenGLCheckFramebuffer(__FILE__, __LINE__);
    }
    
    return(Result);
}

INTERNAL_FUNCTION void OpenGL_DeleteFramebufferPool(opengl_pp_framebuffer_pool* Pool)
{
    
    // NOTE(Dima): Delete framebuffers
    for(int Index = 0;
        Index < Pool->Count;
        Index++)
    {
        opengl_framebuffer* FB = &Pool->Framebuffers[Index].FB;
        
        glDeleteFramebuffers(1, &FB->Framebuffer);
        glDeleteTextures(1, &FB->Texture);
    }
}

INTERNAL_FUNCTION void OpenGLInit(render_commands* Commands, memory_arena* Arena)
{
    opengl_state* OpenGL = PushStruct(Commands->Arena, opengl_state);
    Commands->StateOfGraphicsAPI = OpenGL;
    OpenGL->Arena = Arena;
    
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 
                  &OpenGL->MaxCombinedTextureUnits);
    
    int Width = Commands->WindowDimensions.InitWidth;
    int Height = Commands->WindowDimensions.InitHeight;
    lighting* Lighting = &Commands->Lighting;
    
    glewExperimental = GL_TRUE;
    glewInit();
    
    SDL_GL_SetSwapInterval(0);
    
    glEnable(GL_DEPTH_TEST);
    
    OpenGLInitDefaultObjects(Commands);
    
    // NOTE(Dima): Init framebuffer pools
    OpenGL->FramebufPoolNormalRes = OpenGL_InitFramebufferPool(Commands, 4, 
                                                               Width, 
                                                               Height,
                                                               PostProcessResolution_Normal);
    
    OpenGL->FramebufPoolHalfRes = OpenGL_InitFramebufferPool(Commands, 4, 
                                                             Width / 2, 
                                                             Height / 2,
                                                             PostProcessResolution_Half);
    
    OpenGL->FramebufPoolQuaterRes = OpenGL_InitFramebufferPool(Commands, 4, 
                                                               Width / 4, 
                                                               Height / 4,
                                                               PostProcessResolution_Quater);
    
    // NOTE(Dima): Init GBuffer
    OpenGL_GBufferInit(&OpenGL->GBuffer, Width, Height);
    OpenGL_SSAO_Init(Commands, Width, Height);
    OpenGL_InitShadowMaps(Commands);
    
    OpenGLLoadShaders(Commands, Arena);
}

INTERNAL_FUNCTION void OpenGLFree(render_commands* Commands)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    OpenGL_DeleteFramebufferPool(&OpenGL->FramebufPoolNormalRes);
    OpenGL_DeleteFramebufferPool(&OpenGL->FramebufPoolHalfRes);
    OpenGL_DeleteFramebufferPool(&OpenGL->FramebufPoolQuaterRes);
    
    // NOTE(Dima): Free objects
    OpenGL_DeleteArrayObject(&OpenGL->ScreenQuad);
    OpenGL_DeleteArrayObject(&OpenGL->SkyboxCube);
    
    // NOTE(Dima): Delete GBuffer
    OpenGL_GBufferFree(&OpenGL->GBuffer);
    OpenGL_SSAO_Free(Commands);
    
    // NOTE(Dima): Delete shaders
    opengl_loaded_shader* ShaderAt = OpenGL->LoadedShadersList;
    while(ShaderAt != 0)
    {
        OpenGLDeleteShader(ShaderAt->Shader);
        
        ShaderAt = ShaderAt->NextInLoadedShaderList;
    }
}

INTERNAL_FUNCTION void OpenGLInitMeshAttribs(mesh* Mesh, opengl_shader* Shader)
{
    // NOTE(Dima): Position
    InitAttribFloat(Shader->GetAttribLoc("InPosition"),
                    3, 3 * sizeof(float),
                    Mesh->Offsets.OffsetP);
    
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
}

INTERNAL_FUNCTION renderer_handle* OpenGLAllocateMesh(mesh* Mesh, 
                                                      opengl_shader* Shader1,
                                                      opengl_shader* Shader2)
{
    renderer_handle* Result = &Mesh->Handle;
    
    b32 MeshWasDeleted = OpenGLProcessHandleInvalidation(Result);
    
    if(!Result->Initialized || MeshWasDeleted)
    {
        if(Mesh->FreeSize > 0)
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
                         Mesh->FreeSize, 
                         Mesh->Free, 
                         GL_STATIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                         sizeof(u32) * Mesh->IndexCount, 
                         Mesh->Indices, 
                         GL_STATIC_DRAW);
            
            if(Shader1)
            {
                OpenGLInitMeshAttribs(Mesh, Shader1);
            }
            
            if(Shader2)
            {
                OpenGLInitMeshAttribs(Mesh, Shader2);
            }
            
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

INTERNAL_FUNCTION void OpenGLRenderMesh(render_commands* Commands,
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
    
    OpenGLAllocateMesh(Mesh, 
                       OpenGL->StdShader, 
                       OpenGL->StdShadowShader);
    
    // NOTE(Dima): Render
    glBindVertexArray(Mesh->Handle.Mesh.ArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, Mesh->Handle.Mesh.BufferObject);
    
    Shader->Use();
    
    Shader->SetMat4("ViewProjection", RenderPass->ViewProjection);
    Shader->SetMat4("Projection", RenderPass->Projection.e);
    Shader->SetMat4("View", RenderPass->View.e);
    Shader->SetMat4("Model", InstanceModelTransforms[0].e);
    Shader->SetVec3("MultColor", Color.r, Color.g, Color.b);
    Shader->SetBool("UseInstancing", UseInstancing);
    
    
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
    
    int ModelTranLoc1 = Shader->GetAttribLoc("InInstanceModelTran1");
    int ModelTranLoc2 = Shader->GetAttribLoc("InInstanceModelTran2");
    int ModelTranLoc3 = Shader->GetAttribLoc("InInstanceModelTran3");
    int ModelTranLoc4 = Shader->GetAttribLoc("InInstanceModelTran4");
    
    // NOTE(Dima): Instancing
    GLuint InstanceModelBO;
    GLuint InstanceModelTBO;
    if(UseInstancing)
    {
        glGenBuffers(1, &InstanceModelBO);
        
        // NOTE(Dima): Generating buffer for holding instance model transforms and mesh
        glBindBuffer(GL_ARRAY_BUFFER, InstanceModelBO);
        glBufferData(GL_ARRAY_BUFFER, 
                     sizeof(m44) * MeshInstanceCount + Mesh->FreeSize, 
                     0,
                     GL_STREAM_DRAW);
        
        // NOTE(Dima): Adding mesh to buffer
        if(Mesh->FreeSize > 0)
        {
            glBufferSubData(GL_ARRAY_BUFFER,
                            0,
                            Mesh->FreeSize,
                            Mesh->Free);
        }
        
        // NOTE(Dima): Adding instance model transforms to buffer
        if(MeshInstanceCount)
        {
            glBufferSubData(GL_ARRAY_BUFFER,
                            Mesh->FreeSize,
                            sizeof(m44) * MeshInstanceCount,
                            InstanceModelTransforms);
        }
        
        OpenGLInitMeshAttribs(Mesh, Shader);
        
        // NOTE(Dima): Setting instance model transform attribs
        InitAttribFloat(ModelTranLoc1,
                        4, sizeof(m44),
                        Mesh->FreeSize);
        
        InitAttribFloat(ModelTranLoc2,
                        4, sizeof(m44),
                        Mesh->FreeSize + 1 * sizeof(v4));
        
        InitAttribFloat(ModelTranLoc3,
                        4, sizeof(m44),
                        Mesh->FreeSize + 2 * sizeof(v4));
        
        InitAttribFloat(ModelTranLoc4,
                        4, sizeof(m44),
                        Mesh->FreeSize + 3 * sizeof(v4));
        
        glVertexAttribDivisor(ModelTranLoc1, 1);
        glVertexAttribDivisor(ModelTranLoc2, 1);
        glVertexAttribDivisor(ModelTranLoc3, 1);
        glVertexAttribDivisor(ModelTranLoc4, 1);
    }
    
    // NOTE(Dima): Setting material
    b32 MaterialMissing = true;
    b32 DiffuseWasSet = false;
    
    if(Material != 0)
    {
        MaterialMissing = false;
        
        if(Material->Diffuse != 0)
        {
            DiffuseWasSet = true;
            
            OpenGLInitImage(Material->Diffuse);
            
            Shader->SetTexture2D("TexDiffuse", 
                                 Material->Diffuse->Handle.Image.TextureObject,
                                 0);
        }
    }
    
    Shader->SetBool("MaterialMissing", MaterialMissing);
    Shader->SetBool("HasDiffuse", DiffuseWasSet);
    
    Shader->SetBool("HasClippingPlane", RenderPass->ClippingPlaneIsSet);
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
INTERNAL_FUNCTION void OpenGLRenderVoxelMesh(render_commands* Commands, 
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
    
    b32 MeshWasDeleted = OpenGLProcessHandleInvalidation(&Mesh->Handle);
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
    b32 PerFaceBufWasDeleted = OpenGLProcessHandleInvalidation(&Mesh->PerFaceBufHandle);
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
    GLuint AtlasTexture = OpenGLInitImage(VoxelAtlas);
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

INTERNAL_FUNCTION void OpenGLRenderImage(render_commands* Commands,
                                         image* Image, v2 P, 
                                         f32 Width, f32 Height, 
                                         v4 C)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->UIRectShader;
    
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
    Shader->SetMat4("Projection", OrthographicProjectionWindow(Commands->WindowDimensions.Width,
                                                               Commands->WindowDimensions.Height));
    Shader->SetVec4("MultColor", C.r, C.g, C.b, C.a);
    Shader->SetBool("IsBatch", false);
    
    b32 IsImage = Image != 0;
    if(IsImage)
    {
        OpenGLInitImage(Image);
        
        Shader->SetTexture2D("Image", Image->Handle.Image.TextureObject, 0);
    }
    Shader->SetBool("IsImage", IsImage);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // NOTE(Dima): Free everything we need
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
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

INTERNAL_FUNCTION void OpenGLRenderCommands(render_commands* Commands, render_pass* RenderPass)
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
            case RenderCommand_Mesh:
            {
                render_command_mesh* MeshCommand = GetRenderCommand(Commands, CommandIndex, render_command_mesh);
                
                OpenGLRenderMesh(Commands,
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
                render_command_instanced_mesh* MeshCommand = GetRenderCommand(Commands, CommandIndex, 
                                                                              render_command_instanced_mesh);
                
                OpenGLRenderMesh(Commands,
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
                render_command_voxel_mesh* MeshCommand = GetRenderCommand(Commands, CommandIndex,
                                                                          render_command_voxel_mesh);
                
                voxel_mesh* Mesh = MeshCommand->Mesh;
                
                
                b32 IsCulled = IsFrustumCulled(RenderPass, 
                                               &MeshCommand->CullingInfo, 
                                               Commands->CullingEnabled);
                
                if(!IsCulled)
                {
                    OpenGLRenderVoxelMesh(Commands,
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

INTERNAL_FUNCTION void OpenGL_RenderShadowMaps(render_commands* Commands)
{
    FUNCTION_TIMING();
    
    // NOTE(Dima): DEPTH TEST SHOULD BE ENABLED
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    lighting* Lighting = &Commands->Lighting;
    
    if(Lighting->DirLit.CalculateShadows)
    {
        glEnable(GL_DEPTH_TEST);
        
        glBindFramebuffer(GL_FRAMEBUFFER, OpenGL->ShadowMap.Framebuffer);
        glViewport(0, 0,
                   OpenGL->ShadowMap.Width,
                   OpenGL->ShadowMap.Height);
        
        for(int CascadeIndex = 0;
            CascadeIndex < OpenGL->InitCascadesCount;
            CascadeIndex++)
        {
            shadow_cascade_info* Cascade = &Commands->Lighting.Cascades[CascadeIndex];
            render_pass* ShadowPass = Cascade->RenderPass;
            
            int TargetCascadeIndex = CascadeIndex;
            if(Lighting->BlurVarianceShadowMaps)
            {
                TargetCascadeIndex = OpenGL->InitCascadesCount;
            }
            
            // NOTE(Dima): Now bind layer to copy and create depth squared buffer
            glFramebufferTextureLayer(GL_FRAMEBUFFER,
                                      GL_COLOR_ATTACHMENT0,
                                      OpenGL->ShadowMap.Texture,
                                      0, TargetCascadeIndex);
            
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            
            // NOTE(Dima): Rendering
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            OpenGLRenderCommands(Commands, ShadowPass);
            
            if(Lighting->BlurVarianceShadowMaps)
            {
                glDisable(GL_DEPTH_TEST);
                
                // NOTE(Dima): Apply blur
                glFramebufferTextureLayer(GL_FRAMEBUFFER,
                                          GL_COLOR_ATTACHMENT0,
                                          OpenGL->ShadowMap.Texture,
                                          0, CascadeIndex);
                
                glDrawBuffer(GL_COLOR_ATTACHMENT0);
                
                // NOTE(Dima): Rendering
                //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
                //glClear(GL_COLOR_BUFFER_BIT);
                
                opengl_shader* BlurShader = OpenGL->VarianceShadowBlurShader;
                BlurShader->Use();
                BlurShader->SetTexture2DArray("TextureArray", OpenGL->ShadowMap.Texture, 0);
                BlurShader->SetInt("TextureIndex", OpenGL->InitCascadesCount);
                BlurShader->SetInt("BlurRadius", Lighting->VarianceShadowMapBlurRadius);
                
                glBindVertexArray(OpenGL->ScreenQuad.VAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                
                glEnable(GL_DEPTH_TEST);
            }
        }
    }
}

INTERNAL_FUNCTION opengl_pp_framebuffer* OpenGL_DoLightingPass(render_commands* Commands,
                                                               render_pass* RenderPass,
                                                               pp_resolution Resolution)
{
    FUNCTION_TIMING();
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_ssao* SSAO = &OpenGL->SSAO;
    opengl_shader* LitShader = OpenGL->LightingShader;
    lighting* Lighting = &Commands->Lighting;
    postprocessing* PP = &Commands->PostProcessing;
    
    opengl_pp_framebuffer* Result = OpenGL_BeginPP(Commands, Resolution);
    OpenGL_BindPP(Result);
    
    
    // NOTE(Dima): LIGHTING PASS. Preparing GBuffer
    LitShader->Use();
    
    const m44& Projection = RenderPass->Projection;
    LitShader->SetVec4("PerspProjCoefs",
                       Projection.e[0],
                       Projection.e[5],
                       Projection.e[10],
                       Projection.e[14]);
    
    LitShader->SetVec2("WH", 
                       Commands->WindowDimensions.Width,
                       Commands->WindowDimensions.Height);
    
    LitShader->SetTexture2D("ColorSpecTex",
                            OpenGL->GBuffer.ColorSpec, 0);
    
    LitShader->SetTexture2D("NormalTex",
                            OpenGL->GBuffer.Normal, 1);
    
    LitShader->SetTexture2D("PositionsTex",
                            OpenGL->GBuffer.Positions, 2);
    
    LitShader->SetTexture2D("DepthTex",
                            OpenGL->GBuffer.Depth, 3);
    
    LitShader->SetBool("SSAOEnabled", PP->SSAO_Params.Enabled);
    if(PP->SSAO_Params.Enabled)
    {
        LitShader->SetFloat("SSAOContribution", PP->SSAO_Params.Contribution);
        LitShader->SetTexture2D("SSAOTex",
                                SSAO->BlurFramebufferTexture, 4);
    }
    
    
    // NOTE(Dima): Uniform lighting variables
    LitShader->SetVec3("CameraP", RenderPass->CameraP);
    LitShader->SetFloat("AmbientPercentage", Lighting->AmbientPercentage);
    LitShader->SetFloat("ShadowStrength", Lighting->ShadowStrength);
    
    
    LitShader->SetVec3("DirectionalLightDirection", Lighting->DirLit.Dir);
    LitShader->SetVec3("DirectionalLightColor", Lighting->DirLit.C);
    LitShader->SetBool("CalculateDirLightShadow", Lighting->DirLit.CalculateShadows);
    LitShader->SetTexture2DArray("LightDepthTex", OpenGL->ShadowMap.Texture, 5);
    
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
    
    LitShader->SetVec2Array("PoissonSamples", 
                            Lighting->PCF_PoissonSamples.Samples,
                            Lighting->PCF_PoissonSamples.Count);
    LitShader->SetTexture2D("PoissonSamplesRotations",
                            OpenGL->PoissonSamplesRotationTex,
                            6);
    LitShader->SetBool("ShouldRotateSamples", 
                       Lighting->PCF_PoissonSamples.ShouldRotateSamples);
    
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    OpenGLCheckError(__FILE__, __LINE__);
    
    return(Result);
}

INTERNAL_FUNCTION opengl_pp_framebuffer* OpenGL_RenderPassToGBuffer(render_commands* Commands,
                                                                    render_pass* Pass,
                                                                    pp_resolution Resolution)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    postprocessing* PP = &Commands->PostProcessing;
    
    // NOTE(Dima): Rendering to GBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, OpenGL->GBuffer.Framebuffer);
    glViewport(0, 0,
               OpenGL->GBuffer.Width,
               OpenGL->GBuffer.Height);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    OpenGLRenderCommands(Commands, Pass);
    
    if(PP->SSAO_Params.Enabled)
    {
        OpenGL_SSAO_DoPass(Commands, 
                           &OpenGL->GBuffer,
                           Pass);
    }
    
    opengl_pp_framebuffer* LightingPass = OpenGL_DoLightingPass(Commands, Pass, Resolution);
    
    return(LightingPass);
}

INTERNAL_FUNCTION opengl_pp_framebuffer* OpenGL_RenderWater(render_commands* Commands,
                                                            opengl_pp_framebuffer* Scene,
                                                            opengl_pp_framebuffer* WaterReflection,
                                                            u32 PositionsTex,
                                                            u32 DepthTex,
                                                            render_pass* MainRenderPass)
{
    opengl_pp_framebuffer* Result = OpenGL_BeginPP(Commands, PostProcessResolution_Normal);
    OpenGL_BindPP(Result);
    
    render_water* Water = &Commands->Water;
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = OpenGL->RenderWaterShader;
    
    // NOTE(Dima): Setting shader uniforms
    Shader->Use();
    
    Shader->SetTexture2D("SceneRender", Scene->FB.Texture, 0);
    Shader->SetTexture2D("ScenePositionsTex", PositionsTex, 1);
    Shader->SetTexture2D("ReflectionRender", WaterReflection->FB.Texture, 2);
    Shader->SetTexture2D("SceneDepthTex", DepthTex, 3);
    
    Shader->SetVec4("ClipPlane", Water->PlaneEquation);
    Shader->SetVec3("CameraP", MainRenderPass->CameraP);
    
    Shader->SetVec3("WaterColor", Water->Params.Color.rgb);
    
    // NOTE(Dima): Render quad
    glBindVertexArray(OpenGL->ScreenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    return(Result);
}

INTERNAL_FUNCTION void OpenGLRenderImagesList(render_commands* Commands)
{
    render_command_image* At = Commands->ImageUse.Next;
    
    while(At != &Commands->ImageUse)
    {
        OpenGLRenderImage(Commands,
                          At->Image,
                          At->P,
                          At->Dim.x,
                          At->Dim.y,
                          At->C);
        
        At = At->Next;
    }
}

INTERNAL_FUNCTION void OpenGLRenderRectBuffer(render_commands* Commands, 
                                              batch_rect_buffer* RectBuffer)
{
    // NOTE(Dima): Skip if we have no rectangles to render
    if (RectBuffer->RectCount == 0)
    {
        return;
    }
    
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
    
    b32 IsImage = RectBuffer->TextureAtlas != 0;
    if(IsImage)
    {
        OpenGLInitImage(RectBuffer->TextureAtlas);
        
        Shader->SetTexture2D("Image", RectBuffer->TextureAtlas->Handle.Image.TextureObject, 0);
    }
    Shader->SetBool("IsImage", IsImage);
    
    
    // NOTE(Dima): Creating and binding colors buffer
    renderer_handle ColorsTexBuf = {};
    OpenGLCreateAndBindTextureBuffer(&ColorsTexBuf,
                                     sizeof(u32) * RectBuffer->RectCount,
                                     &RectBuffer->Colors[0],
                                     GL_R32UI,
                                     1,
                                     Shader->GetLoc("RectsColors"));
    
    // NOTE(Dima): Creating and binding geometry types buffer
    renderer_handle TypesTexBuf = {};
    OpenGLCreateAndBindTextureBuffer(&TypesTexBuf,
                                     sizeof(u8) * RectBuffer->RectCount,
                                     &RectBuffer->Types[0],
                                     GL_R8UI, 2, 
                                     Shader->GetLoc("RectsTypes"));
    
    
    glDrawElements(GL_TRIANGLES, RectBuffer->RectCount * 6, GL_UNSIGNED_INT, 0);
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    OpenGLDeleteHandle(&ColorsTexBuf);
    OpenGLDeleteHandle(&TypesTexBuf);
    
    glUseProgram(0);
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
    
    for(int TextureIndex = 0;
        TextureIndex < OpenGL->InitCascadesCount * 2;
        TextureIndex++)
    {
        OpenGL_RenderDepthRect(Commands, 
                               OpenGL->ShadowMap.Texture,
                               TextureIndex);
    }
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

INTERNAL_FUNCTION PLATFORM_RENDERER_RENDER(OpenGLRender)
{
    FUNCTION_TIMING();
    
    OpenGLProcessDeallocList(Commands);
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    postprocessing* PP = &Commands->PostProcessing;
    
    render_pass* Pass = &Commands->RenderPasses[0];
    render_pass* WaterPass = Commands->Water.ReflectionPass;
    
    glEnable(GL_DEPTH_TEST);
    
    // NOTE(Dima): Rendering to shadow buffer
    OpenGL_RenderShadowMaps(Commands);
    
    OpenGLCheckError(__FILE__, __LINE__);
    
    opengl_pp_framebuffer* WaterRenderResult = 0;
    opengl_pp_framebuffer* CombineRenderResult = 0;
    
#if 1    
    opengl_pp_framebuffer* SceneRenderResult = 0; 
    
    // NOTE(Dima): Combining water and scene render pass results
    if(Commands->WaterIsSet)
    {
        WaterRenderResult = OpenGL_RenderPassToGBuffer(Commands, WaterPass, 
                                                       PostProcessResolution_Normal);
        
        // NOTE(Dima): Doing scene pass after water pass so that we have fresh positions texture
        // NOTE(Dima): that we need in water rendering next
        SceneRenderResult = OpenGL_RenderPassToGBuffer(Commands, Pass,
                                                       PostProcessResolution_Normal);
        
        CombineRenderResult = OpenGL_RenderWater(Commands,
                                                 SceneRenderResult,
                                                 WaterRenderResult,
                                                 OpenGL->GBuffer.Positions,
                                                 OpenGL->GBuffer.Depth,
                                                 Pass);
        
        OpenGL_EndPP(SceneRenderResult);
        OpenGL_EndPP(WaterRenderResult);
    }
    else
    {
        SceneRenderResult = OpenGL_RenderPassToGBuffer(Commands, Pass,
                                                       PostProcessResolution_Normal);
        
        CombineRenderResult = SceneRenderResult;
    }
    
#else
    CombineRenderResult = OpenGL_RenderPassToGBuffer(Commands, WaterPass);
#endif
    
#if 1
    // NOTE(Dima): Some post processing
    opengl_pp_framebuffer* LittleBlur = OpenGL_DoBoxBlur(Commands, 
                                                         CombineRenderResult->FB.Texture, 
                                                         4, PostProcessResolution_Quater);
    
    opengl_pp_framebuffer* DepthOfField = OpenGL_DoDepthOfField(Commands,
                                                                CombineRenderResult->FB.Texture,
                                                                LittleBlur->FB.Texture,
                                                                OpenGL->GBuffer.Depth,
                                                                PP_DepthOfFieldDefaultParams());
    
    OpenGL_EndPP(LittleBlur);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, DepthOfField->FB.Framebuffer);
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, LightingPass->FB.Framebuffer);
    
    int Width = Commands->WindowDimensions.Width;
    int Height = Commands->WindowDimensions.Height;
    glBlitFramebuffer(0, 0, Width, Height,
                      0, 0, Width, Height,
                      GL_COLOR_BUFFER_BIT,
                      GL_LINEAR);
    
    OpenGL_EndPP(DepthOfField);
    OpenGL_EndPP(CombineRenderResult);
#else
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, CombineRenderResult->FB.Framebuffer);
    
    int Width = Commands->WindowDimensions.Width;
    int Height = Commands->WindowDimensions.Height;
    glBlitFramebuffer(0, 0, Width, Height,
                      0, 0, Width, Height,
                      GL_COLOR_BUFFER_BIT,
                      GL_LINEAR);
    OpenGL_EndPP(CombineRenderResult);
#endif
    
    
    // NOTE(Dima): Rendering images and UI rect buffer
    glDisable(GL_DEPTH_TEST);
    //OpenGL_RenderDepthRects(Commands);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    OpenGLRenderImagesList(Commands);
    OpenGLRenderRectBuffer(Commands, Commands->Rects2D_Unit);
    OpenGLRenderRectBuffer(Commands, Commands->Rects2D_Window);
    OpenGLRenderRectBuffer(Commands, Commands->Rects3D);
    glDisable(GL_BLEND);
}

INTERNAL_FUNCTION PLATFORM_RENDERER_PRESENT(OpenGLPresent)
{
    SDL_GL_SwapWindow(App->Window);
}