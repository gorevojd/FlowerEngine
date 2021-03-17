inline opengl_state* GetOpenGL(render_commands* Commands)
{
    opengl_state* Result = (opengl_state*)Commands->StateOfGraphicsAPI;
    
    return(Result);
}

void UniformBool(GLint Loc, b32 Value){
    glUniform1i(Loc, Value);
}

void UniformInt(GLint Loc, int Value){
    glUniform1i(Loc, Value);
}


void UniformFloat(GLint Loc, float Value){
    glUniform1f(Loc, Value);
}

void UniformVec2(GLint Loc, float x, float y){
    glUniform2f(Loc, x, y);
}


void UniformVec2(GLint Loc, v2 Vector){
    glUniform2f(Loc, Vector.x, Vector.y);
}

void UniformVec3(GLint Loc, float x, float y, float z){
    glUniform3f(Loc, x, y, z);
}

void UniformVec3(GLint Loc, v3 A){
    glUniform3f(Loc, A.x, A.y, A.z);
}

void UniformVec4(GLint Loc, float x, float y, float z, float w){
    glUniform4f(Loc, x, y, z, w);
}

void UniformVec4(GLint Loc, v4 A){
    glUniform4f(Loc, A.x, A.y, A.z, A.w);
}

void UniformMatrix4x4(GLint Loc, float* Data)
{
    glUniformMatrix4fv(Loc, 1, true, Data);
}

void UniformMatrixArray4x4(GLint Loc, int Count, m44* Array)
{
    glUniformMatrix4fv(Loc, Count, true, (const GLfloat*)Array);
}

inline void UniformTextureInternal(GLint Loc, GLuint Texture, GLint Slot, GLint Target)
{
    glActiveTexture(GL_TEXTURE0 + Slot);
    glBindTexture(Target, Texture);
    glUniform1i(Loc, Slot);
}

void UniformTexture2D(GLint Loc, GLuint Texture, GLint Slot)
{
    UniformTextureInternal(Loc, Texture, Slot, GL_TEXTURE_2D);
}

void UniformTextureBuffer(GLint Loc, GLuint Texture, GLint Slot)
{
    UniformTextureInternal(Loc, Texture, Slot, GL_TEXTURE_BUFFER);
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
        
        stbsp_sprintf(Buf, "%s\n(File: %s, Line: %d)", Text, File, Line);
        
        printf(Buf);
    }
}

INTERNAL_FUNCTION GLuint OpenGLLoadProgram(char* VertexFilePath, 
                                           char* FragmentFilePath, 
                                           char* GeometryFilePath = 0) 
{
    char* VertexSource = Platform.ReadFileAndNullTerminate(VertexFilePath);
    char* FragmentSource = Platform.ReadFileAndNullTerminate(FragmentFilePath);
    char* GeometrySource = Platform.ReadFileAndNullTerminate(GeometryFilePath);
    
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

INTERNAL_FUNCTION opengl_shader OpenGLLoadShader(char* ShaderName, 
                                                 char* VertexFilePath, 
                                                 char* FragmentFilePath, 
                                                 char* GeometryFilePath = 0)
{
    opengl_shader Result = {};
    
    Result.ID = OpenGLLoadProgram(VertexFilePath, 
                                  FragmentFilePath,
                                  GeometryFilePath);
    
    if(Result.ID == -1)
    {
        SDL_Log("%s shader loaded successfully!\n", ShaderName);
    }
    
    CopyStringsSafe(Result.Name, ArrayCount(Result.Name), ShaderName);
    
    // NOTE(Dima): Loading attributes
    OPENGL_LOAD_ATTRIB(Position);
    OPENGL_LOAD_ATTRIB(TexCoords);
    OPENGL_LOAD_ATTRIB(Normal);
    OPENGL_LOAD_ATTRIB(Color);
    OPENGL_LOAD_ATTRIB(Weights);
    OPENGL_LOAD_ATTRIB(BoneIDs);
    OPENGL_LOAD_ATTRIB(PosUV);
    
    OPENGL_LOAD_ATTRIB(InstanceModelTran1);
    OPENGL_LOAD_ATTRIB(InstanceModelTran2);
    OPENGL_LOAD_ATTRIB(InstanceModelTran3);
    OPENGL_LOAD_ATTRIB(InstanceModelTran4);
    
    // NOTE(Dima): Loading uniforms
    OPENGL_LOAD_UNIFORM(ViewProjection);
    OPENGL_LOAD_UNIFORM(Projection);
    OPENGL_LOAD_UNIFORM(View);
    OPENGL_LOAD_UNIFORM(Model);
    OPENGL_LOAD_UNIFORM(SkinningMatrices);
    OPENGL_LOAD_UNIFORM(SkinningMatricesCount);
    OPENGL_LOAD_UNIFORM(MeshIsSkinned);
    OPENGL_LOAD_UNIFORM(UseInstancing);
    
    OPENGL_LOAD_UNIFORM(MultColor);
    OPENGL_LOAD_UNIFORM(TexDiffuse);
    OPENGL_LOAD_UNIFORM(HasDiffuse);
    OPENGL_LOAD_UNIFORM(MaterialMissing);
    
    // NOTE(Dima): Uniforms for voxel shader
    OPENGL_LOAD_UNIFORM(TextureAtlas);
    OPENGL_LOAD_UNIFORM(ChunkAt);
    OPENGL_LOAD_UNIFORM(PerFaceData);
    
    // NOTE(Dima): Loading uniforms for text rendering
    OPENGL_LOAD_UNIFORM(IsImage);
    OPENGL_LOAD_UNIFORM(Image);
    OPENGL_LOAD_UNIFORM(RectsColors);
    OPENGL_LOAD_UNIFORM(RectsTypes);
    OPENGL_LOAD_UNIFORM(IsBatch);
    
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


INTERNAL_FUNCTION void OpenGL_BindPP(opengl_framebuffer* Framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer->Framebuffer);
    
    glViewport(0, 0, 
               Framebuffer->Width,
               Framebuffer->Height);
}

INTERNAL_FUNCTION opengl_framebuffer OpenGL_BeginPP(render_commands* Commands)
{
    opengl_framebuffer Result = {};
    
    Result.Width = Commands->WindowDimensions.Width;
    Result.Height = Commands->WindowDimensions.Height;
    
    // NOTE(Dima): Generating framebuffer
    glGenFramebuffers(1, &Result.Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Result.Framebuffer);
    
    // NOTE(Dima): Generating texture attachment
    glGenTextures(1, &Result.Texture);
    glBindTexture(GL_TEXTURE_2D, Result.Texture);
    glTexImage2D(GL_TEXTURE_2D, 0, 
                 GL_RGB8, 
                 Commands->WindowDimensions.Width, 
                 Commands->WindowDimensions.Height, 
                 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, 
                           GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_2D, 
                           Result.Texture, 0);
    
    return(Result);
}

INTERNAL_FUNCTION void OpenGL_EndPP(opengl_framebuffer* Framebuffer)
{
    glDeleteFramebuffers(1, &Framebuffer->Framebuffer);
    glDeleteTextures(1, &Framebuffer->Texture);
}

INTERNAL_FUNCTION opengl_framebuffer OpenGL_DoDilation(render_commands* Commands, 
                                                       u32 InputTexture,
                                                       pp_dilation_params Params)
{
    opengl_framebuffer Result = OpenGL_BeginPP(Commands);
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = &OpenGL->DilationShader;
    
    Shader->Use();
    Shader->SetInt("Size", Params.Size);
    Shader->SetVec2("MinMaxThreshold", 
                    Params.MinThreshold, 
                    Params.MaxThreshold);
    Shader->SetTexture2D("InputTexture", 
                         InputTexture, 0);
    
    glBindVertexArray(OpenGL->ScreenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    return(Result);
}

INTERNAL_FUNCTION opengl_framebuffer OpenGL_DoPosterize(render_commands* Commands,
                                                        u32 InputTexture,
                                                        int Levels)
{
    opengl_framebuffer Result = OpenGL_BeginPP(Commands);
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = &OpenGL->PosterizeShader;
    
    Shader->Use();
    Shader->SetInt("Levels", Levels);
    Shader->SetTexture2D("InputTexture", 
                         InputTexture, 0);
    
    glBindVertexArray(OpenGL->ScreenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    return(Result);
}

INTERNAL_FUNCTION opengl_framebuffer OpenGL_DoBoxBlur(render_commands* Commands,
                                                      u32 InputTexture,
                                                      int RadiusSize)
{
    opengl_framebuffer Result = OpenGL_BeginPP(Commands);
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = &OpenGL->BoxBlurShader;
    
    Shader->Use();
    Shader->SetInt("BlurRadius", RadiusSize);
    Shader->SetTexture2D("ToBlurTex", 
                         InputTexture, 0);
    
    glBindVertexArray(OpenGL->ScreenQuadVAO);
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
    opengl_shader* Shader = &OpenGL->SSAOShader;
    postprocessing* PP = &Commands->PostProcessing;
    const m44& Projection = RenderPass->Projection;
    
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
                 Commands->WindowDimensions.Width, 
                 Commands->WindowDimensions.Height, 
                 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, 
                           GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_2D, 
                           SSAO->FramebufferTexture, 0);
    
    // NOTE(Dima): Do pass
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    Shader->Use();
    
    Shader->SetTexture2D("DepthTex", GBuf->Depth, 0);
    Shader->SetTexture2D("NormalTex", GBuf->Normal, 1);
    Shader->SetTexture2D("SSAONoiseTex", SSAO->NoiseTex, 2);
    
    Shader->SetVec3Array("SSAOKernel", 
                         PP->SSAO_Kernel, 
                         PP->SSAO_Params.KernelSize);
    Shader->SetInt("SSAOKernelSamplesCount", PP->SSAO_Params.KernelSize);
    Shader->SetFloat("SSAOKernelRadius", PP->SSAO_Params.KernelRadius);
    Shader->SetFloat("SSAOContribution", PP->SSAO_Params.Contribution);
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
    
    glBindVertexArray(OpenGL->ScreenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
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
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    Shader = &OpenGL->SSAOBlurShader;
    Shader->Use();
    Shader->SetTexture2D("OcclusionTex", SSAO->FramebufferTexture, 0);
    Shader->SetInt("BlurRadius", PP->SSAO_Params.BlurRadius);
    
    glBindVertexArray(OpenGL->ScreenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
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
    glDeleteFramebuffers(1, &GBuf->Framebuffer);
    
    glDeleteTextures(1, &GBuf->ColorSpec);
    glDeleteTextures(1, &GBuf->Normal);
    glDeleteTextures(1, &GBuf->Positions);
    glDeleteTextures(1, &GBuf->Depth);
}

INTERNAL_FUNCTION void OpenGLInit(render_commands* Commands)
{
    opengl_state* OpenGL = PushStruct(Commands->Arena, opengl_state);
    Commands->StateOfGraphicsAPI = OpenGL;
    
    int Width = Commands->WindowDimensions.InitWidth;
    int Height = Commands->WindowDimensions.InitHeight;
    lighting* Lighting = &Commands->Lighting;
    
    glewExperimental = GL_TRUE;
    glewInit();
    
    SDL_GL_SetSwapInterval(0);
    
    glEnable(GL_DEPTH_TEST);
    
    // NOTE(Dima): Init screen Quad
    f32 ScreenQuadData[] = 
    {
        -1.0f , 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        
        -1.0f , 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
    };
    
    glGenVertexArrays(1, &OpenGL->ScreenQuadVAO);
    glGenBuffers(1, &OpenGL->ScreenQuadVBO);
    
    glBindVertexArray(OpenGL->ScreenQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL->ScreenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenQuadData), ScreenQuadData, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, 0, 4 * sizeof(float), 0);
    glBindVertexArray(0);
    
    // NOTE(Dima): Init shaders
    OpenGL->StdShader = OpenGLLoadShader("Standard",
                                         "../Data/Shaders/std.vs",
                                         "../Data/Shaders/std.fs");
    
    OpenGL->UIRectShader = OpenGLLoadShader("UIRect",
                                            "../Data/Shaders/ui_rect.vs",
                                            "../Data/Shaders/ui_rect.fs");
    
    OpenGL->VoxelShader = OpenGLLoadShader("Voxel",
                                           "../Data/Shaders/voxel.vs",
                                           "../Data/Shaders/voxel.fs");
    
    OpenGL->SSAOShader = OpenGLLoadShader("SSAO",
                                          "../Data/Shaders/screen.vs",
                                          "../Data/Shaders/ssao.fs");
    
    OpenGL->SSAOBlurShader = OpenGLLoadShader("SSAOBlur",
                                              "../Data/Shaders/screen.vs",
                                              "../Data/Shaders/ssao_blur.fs");
    
    OpenGL->LightingShader = OpenGLLoadShader("Lighting",
                                              "../Data/Shaders/screen.vs",
                                              "../Data/Shaders/lighting.fs");
    
    OpenGL->BoxBlurShader = OpenGLLoadShader("BoxBlur",
                                             "../Data/Shaders/screen.vs",
                                             "../Data/Shaders/box_blur.fs");
    
    OpenGL->DilationShader = OpenGLLoadShader("Dilation",
                                              "../Data/Shaders/screen.vs",
                                              "../Data/Shaders/dilation.fs");
    
    OpenGL->PosterizeShader = OpenGLLoadShader("Posterize",
                                               "../Data/Shaders/screen.vs",
                                               "../Data/Shaders/posterize.fs");
}

INTERNAL_FUNCTION void OpenGLFree(render_commands* Commands)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    
    OpenGLDeleteShader(&OpenGL->StdShader);
}

INTERNAL_FUNCTION void OpenGLInitMeshAttribs(mesh* Mesh, opengl_shader* Shader)
{
    // NOTE(Dima): Position
    InitAttribFloat(Shader->PositionAttr,
                    3, 3 * sizeof(float),
                    Mesh->Offsets.OffsetP);
    
    // NOTE(Dima): TexCoords
    InitAttribFloat(Shader->TexCoordsAttr,
                    2, 2 * sizeof(float),
                    Mesh->Offsets.OffsetUV);
    
    // NOTE(Dima): Normal
    InitAttribFloat(Shader->NormalAttr,
                    3, 3 * sizeof(float),
                    Mesh->Offsets.OffsetN);
    
    // NOTE(Dima): Color
    InitAttribInt(Shader->ColorAttr,
                  1, sizeof(u32),
                  Mesh->Offsets.OffsetC);
    
    
    // NOTE(Dima): Setting skinning info
    if(Mesh->IsSkinned)
    {
        // NOTE(Dima): Bone weights
        InitAttribFloat(Shader->WeightsAttr,
                        4, 4 * sizeof(float),
                        Mesh->Offsets.OffsetBoneWeights);
        
        // NOTE(Dima): Bone indices
        InitAttribFloat(Shader->BoneIDsAttr,
                        1, sizeof(u32),
                        Mesh->Offsets.OffsetBoneIndices);
    }
}

INTERNAL_FUNCTION renderer_handle* OpenGLAllocateMesh(mesh* Mesh, opengl_shader* Shader)
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
            
            OpenGLInitMeshAttribs(Mesh, Shader);
            
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
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = &OpenGL->StdShader;
    
    OpenGLAllocateMesh(Mesh, Shader);
    
    // NOTE(Dima): Render
    glBindVertexArray(Mesh->Handle.Mesh.ArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, Mesh->Handle.Mesh.BufferObject);
    
    glUseProgram(Shader->ID);
    
    glUniformMatrix4fv(Shader->ViewProjectionLoc, 1, GL_TRUE, RenderPass->ViewProjection.e);
    glUniformMatrix4fv(Shader->ProjectionLoc, 1, GL_TRUE, RenderPass->Projection.e);
    glUniformMatrix4fv(Shader->ViewLoc, 1, GL_TRUE, RenderPass->View.e);
    glUniformMatrix4fv(Shader->ModelLoc, 1, GL_TRUE, InstanceModelTransforms[0].e);
    glUniform3f(Shader->MultColorLoc, Color.r, Color.g, Color.b);
    
    glUniform1i(Shader->UseInstancingLoc, UseInstancing);
    
    // NOTE(Dima): Uniform skinning matrices
    renderer_handle SkinningMatricesTexBuf = {};
    if(Mesh->IsSkinned)
    {
        OpenGLCreateAndBindTextureBuffer(&SkinningMatricesTexBuf,
                                         sizeof(m44) * NumInstanceSkMat * MeshInstanceCount,
                                         SkinningMatrices,
                                         GL_RGBA32F,
                                         1, 
                                         Shader->SkinningMatricesLoc);
    }
    glUniform1i(Shader->SkinningMatricesCountLoc, NumInstanceSkMat);
    glUniform1i(Shader->MeshIsSkinnedLoc, Mesh->IsSkinned);
    
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
        InitAttribFloat(Shader->InstanceModelTran1Attr,
                        4, sizeof(m44),
                        Mesh->FreeSize);
        
        InitAttribFloat(Shader->InstanceModelTran2Attr,
                        4, sizeof(m44),
                        Mesh->FreeSize + 1 * sizeof(v4));
        
        InitAttribFloat(Shader->InstanceModelTran3Attr,
                        4, sizeof(m44),
                        Mesh->FreeSize + 2 * sizeof(v4));
        
        InitAttribFloat(Shader->InstanceModelTran4Attr,
                        4, sizeof(m44),
                        Mesh->FreeSize + 3 * sizeof(v4));
        
        glVertexAttribDivisor(Shader->InstanceModelTran1Attr, 1);
        glVertexAttribDivisor(Shader->InstanceModelTran2Attr, 1);
        glVertexAttribDivisor(Shader->InstanceModelTran3Attr, 1);
        glVertexAttribDivisor(Shader->InstanceModelTran4Attr, 1);
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
            glUniform1i(Shader->TexDiffuseLoc, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Material->Diffuse->Handle.Image.TextureObject);
        }
    }
    
    glUniform1i(Shader->MaterialMissingLoc, MaterialMissing);
    glUniform1i(Shader->HasDiffuseLoc, DiffuseWasSet);
    
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
    
    glUseProgram(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    OpenGLDeleteHandle(&SkinningMatricesTexBuf);
    
    if(UseInstancing)
    {
        glDisableVertexAttribArray(Shader->InstanceModelTran1Attr);
        glDisableVertexAttribArray(Shader->InstanceModelTran2Attr);
        glDisableVertexAttribArray(Shader->InstanceModelTran3Attr);
        glDisableVertexAttribArray(Shader->InstanceModelTran4Attr);
        
        glDeleteBuffers(1, &InstanceModelBO);
    }
}

INTERNAL_FUNCTION void OpenGLRenderVoxelMesh(render_commands* Commands, 
                                             render_pass* RenderPass,
                                             render_command_voxel_mesh* Command,
                                             image* VoxelAtlas)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = &OpenGL->VoxelShader;
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
            
            InitAttribInt(Shader->PositionAttr,
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
    
    glUniformMatrix4fv(Shader->ViewProjectionLoc, 1, GL_TRUE, RenderPass->ViewProjection.e);
    glUniformMatrix4fv(Shader->ProjectionLoc, 1, GL_TRUE, RenderPass->Projection.e);
    glUniformMatrix4fv(Shader->ViewLoc, 1, GL_TRUE, RenderPass->View.e);
    
    // NOTE(Dima): Uniform chunk location
    glUniform3f(Shader->ChunkAtLoc, 
                Command->ChunkAt.x,
                Command->ChunkAt.y,
                Command->ChunkAt.z);
    
    // NOTE(Dima): Uniform voxel atlas
    GLuint AtlasTexture = OpenGLInitImage(VoxelAtlas);
    
    glActiveTexture(GL_TEXTURE0 + VOXEL_MESH_ATLAS_TEXTURE_UNIT);
    glBindTexture(GL_TEXTURE_2D, AtlasTexture);
    glUniform1i(Shader->TextureAtlasLoc, VOXEL_MESH_ATLAS_TEXTURE_UNIT);
    
    // NOTE(Dima): Uniform per-face data
    OpenGLBindTextureBuffer(&Mesh->PerFaceBufHandle,
                            VOXEL_MESH_PER_FACE_TEXTURE_UNIT,
                            Shader->PerFaceDataLoc);
    
    glDrawArrays(GL_TRIANGLES, 0, Mesh->VerticesCount);
    
    glUseProgram(0);
    glBindVertexArray(0);
}

INTERNAL_FUNCTION void OpenGLRenderImage(render_commands* Commands,
                                         image* Image, v2 P, 
                                         f32 Width, f32 Height, 
                                         v4 C)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = &OpenGL->UIRectShader;
    
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
    
    InitAttribFloat(Shader->PosUVAttr, 4, 4 * sizeof(float), 0);
    
    // NOTE(Dima): Using program and setting uniforms
    glUseProgram(Shader->ID);
    glUniformMatrix4fv(Shader->ProjectionLoc, 1, GL_TRUE, Commands->ScreenOrthoProjection.e);
    glUniform4f(Shader->MultColorLoc, C.r, C.g, C.b, C.a);
    glUniform1i(Shader->IsBatchLoc, false);
    
    b32 IsImage = Image != 0;
    if(IsImage)
    {
        OpenGLInitImage(Image);
        
        UniformTexture2D(Shader->ImageLoc, 
                         Image->Handle.Image.TextureObject, 0);
    }
    glUniform1i(Shader->IsImageLoc, IsImage);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glUseProgram(0);
    
    glBindVertexArray(0);
    
    // NOTE(Dima): Free everything we need
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

INTERNAL_FUNCTION void OpenGLRenderGBufferPass(render_commands* Commands, render_pass* RenderPass)
{
    
}

INTERNAL_FUNCTION void OpenGLRenderCommands(render_commands* Commands, render_pass* RenderPass)
{
    for(int CommandIndex = 0;
        CommandIndex < Commands->CommandCount;
        CommandIndex++)
    {
        render_command_header* Header = &Commands->CommandHeaders[CommandIndex];
        
        switch(Header->CommandType)
        {
            case RenderCommand_Clear:
            {
                render_command_clear* ClearCommand = GetRenderCommand(Commands, CommandIndex, render_command_clear);
                
                u32 OpenGLFlags = 0;
                u32 OurFlags = ClearCommand->Flags;
                
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
                
                glClearColor(ClearCommand->C.r,
                             ClearCommand->C.g,
                             ClearCommand->C.b,
                             1.0f);
                
                glClear(OpenGLFlags);
            }break;
            
            case RenderCommand_Mesh:
            {
                render_command_mesh* MeshCommand = GetRenderCommand(Commands, CommandIndex, render_command_mesh);
                
                OpenGLRenderMesh(Commands,
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
                
                OpenGLRenderVoxelMesh(Commands,
                                      RenderPass,
                                      MeshCommand,
                                      Commands->VoxelAtlas);
            }break;
        }
    }
}


INTERNAL_FUNCTION opengl_framebuffer OpenGL_DoLightingPass(render_commands* Commands)
{
    FUNCTION_TIMING();
    
    opengl_framebuffer Result = OpenGL_BeginPP(Commands);
    
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_ssao* SSAO = &OpenGL->SSAO;
    opengl_shader* LitShader = &OpenGL->LightingShader;
    lighting* Lighting = &Commands->Lighting;
    postprocessing* PP = &Commands->PostProcessing;
    render_pass* RenderPass = &Commands->RenderPasses[0];
    const m44& Projection = RenderPass->Projection;
    
    // NOTE(Dima): LIGHTING PASS. Preparing GBuffer
    LitShader->Use();
    
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
        LitShader->SetTexture2D("SSAOTex",
                                SSAO->BlurFramebufferTexture, 4);
    }
    
    // NOTE(Dima): Uniform lighting variables
    LitShader->SetVec3("CameraP", RenderPass->CameraP);
    LitShader->SetVec3("DirectionalLightDirection", Lighting->DirLit.Dir);
    LitShader->SetVec3("DirectionalLightColor", Lighting->DirLit.C);
    
    glBindVertexArray(OpenGL->ScreenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
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
                                              rect_buffer* RectBuffer)
{
    opengl_state* OpenGL = GetOpenGL(Commands);
    opengl_shader* Shader = &OpenGL->UIRectShader;
    
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    OpenGLCheckError(__FILE__, __LINE__);
    
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
    
    InitAttribFloat(Shader->PosUVAttr, 4, 4 * sizeof(float), 0);
    
    glUseProgram(Shader->ID);
    
    glUniformMatrix4fv(Shader->ProjectionLoc, 1, GL_TRUE, Commands->ScreenOrthoProjection.e);
    glUniform4f(Shader->MultColorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform1i(Shader->IsBatchLoc, true);
    
    b32 IsImage = Commands->FontAtlas != 0;
    if(IsImage)
    {
        OpenGLInitImage(Commands->FontAtlas);
        
        UniformTexture2D(Shader->ImageLoc,
                         Commands->FontAtlas->Handle.Image.TextureObject, 0);
    }
    glUniform1i(Shader->IsImageLoc, IsImage);
    
    // NOTE(Dima): Creating and binding colors buffer
    renderer_handle ColorsTexBuf = {};
    OpenGLCreateAndBindTextureBuffer(&ColorsTexBuf,
                                     sizeof(u32) * RectBuffer->RectCount,
                                     &RectBuffer->Colors[0],
                                     GL_R32UI,
                                     1, Shader->RectsColorsLoc);
    
    // NOTE(Dima): Creating and binding geometry types buffer
    renderer_handle TypesTexBuf = {};
    OpenGLCreateAndBindTextureBuffer(&TypesTexBuf,
                                     sizeof(u8) * RectBuffer->RectCount,
                                     &RectBuffer->Types[0],
                                     GL_R8UI, 2, 
                                     Shader->RectsTypesLoc);
    
    glDrawElements(GL_TRIANGLES, RectBuffer->RectCount * 6, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(OpenGL->ScreenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glBindVertexArray(0);
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    OpenGLDeleteHandle(&ColorsTexBuf);
    OpenGLDeleteHandle(&TypesTexBuf);
    
    glUseProgram(0);
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
    
    // NOTE(Dima): Culling
    if(Commands->BackfaceCulling && Commands->BackfaceCullingChanged)
    {
        Commands->BackfaceCullingChanged = false;
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CW);
    }
    
    if(!Commands->BackfaceCulling && Commands->BackfaceCullingChanged)
    {
        Commands->BackfaceCullingChanged = false;
        
        glDisable(GL_CULL_FACE);
    }
    
    // NOTE(Dima): Init GBuffer
    OpenGL_GBufferInit(&OpenGL->GBuffer, 
                       Commands->WindowDimensions.Width,
                       Commands->WindowDimensions.Height);
    
    glBindFramebuffer(GL_FRAMEBUFFER, OpenGL->GBuffer.Framebuffer);
    glViewport(0, 0, 
               Commands->WindowDimensions.Width,
               Commands->WindowDimensions.Height);
    
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // NOTE(Dima): Rendering to GBuffer
    OpenGLRenderCommands(Commands, &Commands->RenderPasses[0]);
    
    if(PP->SSAO_Params.Enabled)
    {
        OpenGL_SSAO_DoPass(Commands, 
                           &OpenGL->GBuffer,
                           &Commands->RenderPasses[0]);
    }
    
    opengl_framebuffer LightingPass = OpenGL_DoLightingPass(Commands);
    
    if(PP->SSAO_Params.Enabled)
    {
        OpenGL_SSAO_Free(Commands);
    }
    
    opengl_framebuffer LittleBlur = OpenGL_DoBoxBlur(Commands, LightingPass.Texture, 3);
    
    pp_dilation_params DilationParams = PP_DilationDefaultParams();
    opengl_framebuffer Dilation = OpenGL_DoDilation(Commands, LittleBlur.Texture, DilationParams);
    OpenGL_EndPP(&LittleBlur);
    
    opengl_framebuffer DilationBlurred = OpenGL_DoBoxBlur(Commands, Dilation.Texture, 4);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, DilationBlurred.Framebuffer);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, Dilation.Framebuffer);
    
    int Width = Commands->WindowDimensions.Width;
    int Height = Commands->WindowDimensions.Height;
    glBlitFramebuffer(0, 0, Width, Height,
                      0, 0, Width, Height,
                      GL_COLOR_BUFFER_BIT,
                      GL_LINEAR);
    OpenGL_EndPP(&Dilation);
    OpenGL_EndPP(&DilationBlurred);
    
    OpenGL_EndPP(&LightingPass);
    OpenGL_GBufferFree(&OpenGL->GBuffer);
    
    //OpenGL_DoDepthOfField(Commands);
    
    // NOTE(Dima): Rendering images and UI rect buffer
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    OpenGLRenderImagesList(Commands);
    OpenGLRenderRectBuffer(Commands, &Commands->Rects2D);
    
    glDisable(GL_BLEND);
}

INTERNAL_FUNCTION PLATFORM_RENDERER_SWAPBUFFERS(OpenGLSwapBuffers)
{
    SDL_GL_SwapWindow(App->Window);
}