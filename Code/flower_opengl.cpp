GLOBAL_VARIABLE opengl_state OpenGL;

#if 0
char* ReadFileAndNullTerminate(char* Path)
{
    if(Path == 0)
    {
        return 0;
    }
    
    SDL_RWops* File = SDL_RWFromFile(Path, "r");
    
    char* Text = 0;
    if(File)
    {
        i64 FileSize = SDL_RWsize(File);
        
        Text = (char*)malloc(FileSize + 1);
        
        SDL_RWread(File, Text, FileSize, 1);
        
        Text[FileSize] = 0;
        
        SDL_RWclose(File);
    }
    
    return(Text);
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
    
    Result.ID = OpenGLLoadProgram( VertexFilePath, 
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
    
    OPENGL_LOAD_UNIFORM(IsImage);
    OPENGL_LOAD_UNIFORM(Image);
    OPENGL_LOAD_UNIFORM(RectsColors);
    OPENGL_LOAD_UNIFORM(RectsTypes);
    OPENGL_LOAD_UNIFORM(Is3D);
    OPENGL_LOAD_UNIFORM(IsBatch);
    OPENGL_LOAD_UNIFORM(RectsTransforms);
    OPENGL_LOAD_UNIFORM(RectsIndicesToTransforms);
    OPENGL_LOAD_UNIFORM(RectOrthoMatrixIndex);
    OPENGL_LOAD_UNIFORM(RectPerspMatrixIndex);
    
    
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
                                                 int TextureUnitIndex,
                                                 GLuint DataFormat,
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
                              TextureUnitIndex,
                              DataFormat);
    
    OpenGLBindTextureBuffer(Handle, 
                            TextureUnitIndex,
                            UniformLoc);
}

INTERNAL_FUNCTION void OpenGLInit()
{
    glewExperimental = GL_TRUE;
    glewInit();
    
    SDL_GL_SetSwapInterval(0);
    
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    
    OpenGL.StdShader = OpenGLLoadShader("Standard",
                                        "../Data/Shaders/std.vs",
                                        "../Data/Shaders/std.fs");
    
    OpenGL.UIRectShader = OpenGLLoadShader("UIRect",
                                           "../Data/Shaders/ui_rect.vs",
                                           "../Data/Shaders/ui_rect.fs");
    
    OpenGL.VoxelShader = OpenGLLoadShader("Voxel",
                                          "../Data/Shaders/voxel.vs",
                                          "../Data/Shaders/voxel.fs");
}

INTERNAL_FUNCTION void OpenGLFree()
{
    OpenGLDeleteShader(&OpenGL.StdShader);
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

INTERNAL_FUNCTION void OpenGLRenderMesh(opengl_shader* Shader,
                                        mesh* Mesh,
                                        material* Material,
                                        v3 Color,
                                        m44* SkinningMatrices,
                                        int NumInstanceSkMat,
                                        m44* View, 
                                        m44* Projection,
                                        m44* InstanceModelTransforms,
                                        int MeshInstanceCount,
                                        b32 UseInstancing)
{
    OpenGLAllocateMesh(Mesh, &OpenGL.StdShader);
    
    // NOTE(Dima): Render
    glBindVertexArray(Mesh->Handle.Mesh.ArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, Mesh->Handle.Mesh.BufferObject);
    
    glUseProgram(OpenGL.StdShader.ID);
    
    m44 ViewProjection = *View * *Projection;
    glUniformMatrix4fv(Shader->ViewProjectionLoc, 1, GL_TRUE, ViewProjection.e);
    glUniformMatrix4fv(Shader->ProjectionLoc, 1, GL_TRUE, Projection->e);
    glUniformMatrix4fv(Shader->ViewLoc, 1, GL_TRUE, View->e);
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

INTERNAL_FUNCTION void OpenGLRenderVoxelMesh(render_command_voxel_mesh* Command,
                                             image* VoxelAtlas,
                                             m44* View,
                                             m44* Projection)
{
    opengl_shader* Shader = &OpenGL.VoxelShader;
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
                                      VOXEL_MESH_PER_FACE_TEXTURE_UNIT,
                                      GL_R32UI,
                                      true);
        }
    }
    
    glBindVertexArray(Mesh->Handle.Mesh.ArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, Mesh->Handle.Mesh.BufferObject);
    glUseProgram(Shader->ID);
    
    m44 ViewProjection = *View * *Projection;
    glUniformMatrix4fv(Shader->ViewProjectionLoc, 1, GL_TRUE, ViewProjection.e);
    glUniformMatrix4fv(Shader->ProjectionLoc, 1, GL_TRUE, Projection->e);
    glUniformMatrix4fv(Shader->ViewLoc, 1, GL_TRUE, View->e);
    
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

INTERNAL_FUNCTION void OpenGLRenderImage(opengl_shader* Shader,
                                         image* Image, v2 P, 
                                         f32 Width, f32 Height, 
                                         v4 C, m44* Projection)
{
    glDisable(GL_DEPTH_TEST);
    
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
    
    // NOTE(Dima): Position
    if(OpenGLAttribIsValid(OpenGL.UIRectShader.PositionAttr))
    {
        glEnableVertexAttribArray(OpenGL.UIRectShader.PositionAttr);
        glVertexAttribPointer(OpenGL.UIRectShader.PositionAttr,
                              2, 
                              GL_FLOAT, 
                              GL_FALSE,
                              4* sizeof(float), 
                              0);
    }
    
    // NOTE(Dima): TexCoords
    if(OpenGLAttribIsValid(OpenGL.UIRectShader.TexCoordsAttr))
    {
        glEnableVertexAttribArray(OpenGL.UIRectShader.TexCoordsAttr);
        glVertexAttribPointer(OpenGL.UIRectShader.TexCoordsAttr,
                              2, 
                              GL_FLOAT, 
                              GL_FALSE,
                              4* sizeof(float), 
                              (void*)(2 * sizeof(float)));
    }
    
    glUseProgram(OpenGL.UIRectShader.ID);
    
    m44 TransformsBuffer[] = {
        *Projection,
        IdentityMatrix4(),
    };
    
    u16 IndicesToTransform[1] = {1};
    
    // NOTE(Dima): Creating and binding Rects transforms buffer
    renderer_handle TransTexBuf = {};
    OpenGLCreateAndBindTextureBuffer(&TransTexBuf,
                                     sizeof(TransformsBuffer),
                                     &TransformsBuffer[0],
                                     GL_RGBA32F,
                                     3, Shader->RectsTransformsLoc);
    
    // NOTE(Dima): Creating and binding indices to transforms buffer
    renderer_handle IndexToTranTexBuf = {};
    OpenGLCreateAndBindTextureBuffer(&IndexToTranTexBuf,
                                     sizeof(IndicesToTransform),
                                     &IndicesToTransform[0],
                                     GL_R16UI,
                                     4, Shader->RectsIndicesToTransformsLoc);
    
    glUniform1i(Shader->RectOrthoMatrixIndexLoc, 0);
    glUniform1i(Shader->RectPerspMatrixIndexLoc, 0);
    
    glUniformMatrix4fv(Shader->ProjectionLoc, 1, GL_TRUE, Projection->e);
    glUniform4f(Shader->MultColorLoc, C.r, C.g, C.b, C.a);
    glUniform1i(Shader->IsBatchLoc, false);
    glUniform1i(Shader->Is3DLoc, false);
    
    b32 IsImage = Image != 0;
    if(IsImage)
    {
        OpenGLInitImage(Image);
        
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(Shader->ImageLoc, 0);
        glBindTexture(GL_TEXTURE_2D, Image->Handle.Image.TextureObject);
    }
    glUniform1i(Shader->IsImageLoc, IsImage);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glUseProgram(0);
    
    glBindVertexArray(0);
    
    // NOTE(Dima): Free everything we need
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    OpenGLDeleteHandle(&TransTexBuf);
    OpenGLDeleteHandle(&IndexToTranTexBuf);
    
    glEnable(GL_DEPTH_TEST);
}

INTERNAL_FUNCTION void OpenGLRenderCommands(render_commands* Commands)
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
            
            case RenderCommand_Image:
            {
                render_command_image* ImageCommand = GetRenderCommand(Commands, CommandIndex, render_command_image);
                
                OpenGLRenderImage(&OpenGL.UIRectShader,
                                  ImageCommand->Image,
                                  ImageCommand->P,
                                  ImageCommand->Dim.x,
                                  ImageCommand->Dim.y,
                                  ImageCommand->C,
                                  &Commands->ScreenOrthoProjection);
            } break;
            
            case RenderCommand_Mesh:
            {
                render_command_mesh* MeshCommand = GetRenderCommand(Commands, CommandIndex, render_command_mesh);
                
                OpenGLRenderMesh(&OpenGL.StdShader,
                                 MeshCommand->Mesh,
                                 MeshCommand->Material,
                                 MeshCommand->C,
                                 MeshCommand->SkinningMatrices,
                                 MeshCommand->SkinningMatricesCount,
                                 &Commands->View, 
                                 &Commands->Projection,
                                 &MeshCommand->ModelToWorld, 1,
                                 false);
            }break;
            
            case RenderCommand_InstancedMesh:
            {
                render_command_instanced_mesh* MeshCommand = GetRenderCommand(Commands, CommandIndex, 
                                                                              render_command_instanced_mesh);
                
                OpenGLRenderMesh(&OpenGL.StdShader,
                                 MeshCommand->Mesh,
                                 MeshCommand->Material,
                                 MeshCommand->C,
                                 MeshCommand->InstanceSkinningMatrices,
                                 MeshCommand->NumSkinningMatricesPerInstance,
                                 &Commands->View, 
                                 &Commands->Projection,
                                 MeshCommand->InstanceMatrices, 
                                 MeshCommand->InstanceCount,
                                 true);
            }break;
            
            case RenderCommand_VoxelChunkMesh:
            {
                render_command_voxel_mesh* MeshCommand = GetRenderCommand(Commands, CommandIndex,
                                                                          render_command_voxel_mesh);
                
                OpenGLRenderVoxelMesh(MeshCommand,
                                      Commands->VoxelAtlas,
                                      &Commands->View,
                                      &Commands->Projection);
            }break;
        }
    }
}

INTERNAL_FUNCTION void OpenGLRenderRectBuffer(render_commands* Commands, 
                                              rect_buffer* RectBuffer, 
                                              b32 Is3DRender,
                                              b32 UseDepthTest)
{
    opengl_shader* Shader = &OpenGL.UIRectShader;
    
    if(UseDepthTest == false)
    {
        glDisable(GL_DEPTH_TEST);
    }
    
    glUseProgram(OpenGL.UIRectShader.ID);
    
    glUniform4f(Shader->MultColorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform1i(Shader->IsBatchLoc, true);
    glUniform1i(Shader->Is3DLoc, Is3DRender);
    
    glUniform1i(Shader->RectOrthoMatrixIndexLoc, RectBuffer->OrthoMatrixIndex);
    glUniform1i(Shader->RectPerspMatrixIndexLoc, RectBuffer->ViewProjMatrixIndex);
    
    b32 IsImage = Commands->FontAtlas != 0;
    if(IsImage)
    {
        OpenGLInitImage(Commands->FontAtlas);
        
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(Shader->ImageLoc, 0);
        glBindTexture(GL_TEXTURE_2D, Commands->FontAtlas->Handle.Image.TextureObject);
    }
    glUniform1i(Shader->IsImageLoc, IsImage);
    
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
                                     GL_R8UI,
                                     2, Shader->RectsTypesLoc);
    
    // NOTE(Dima): Creating and binding Rects transforms buffer
    renderer_handle TransTexBuf = {};
    OpenGLCreateAndBindTextureBuffer(&TransTexBuf,
                                     sizeof(m44) * RectBuffer->TransformsCount,
                                     &RectBuffer->Transforms[0],
                                     GL_RGBA32F,
                                     3, Shader->RectsTransformsLoc);
    
    // NOTE(Dima): Creating and binding indices to transforms buffer
    renderer_handle IndexToTranTexBuf = {};
    OpenGLCreateAndBindTextureBuffer(&IndexToTranTexBuf,
                                     sizeof(u16) * RectBuffer->RectCount,
                                     &RectBuffer->IndicesToTransforms[0],
                                     GL_R16UI,
                                     4, Shader->RectsIndicesToTransformsLoc);
    
    // NOTE(Dima): Position
    if(OpenGLAttribIsValid(OpenGL.UIRectShader.PositionAttr))
    {
        glEnableVertexAttribArray(OpenGL.UIRectShader.PositionAttr);
        glVertexAttribPointer(OpenGL.UIRectShader.PositionAttr,
                              2, 
                              GL_FLOAT, 
                              GL_FALSE,
                              4* sizeof(float), 
                              0);
    }
    
    // NOTE(Dima): TexCoords
    if(OpenGLAttribIsValid(OpenGL.UIRectShader.TexCoordsAttr))
    {
        glEnableVertexAttribArray(OpenGL.UIRectShader.TexCoordsAttr);
        glVertexAttribPointer(OpenGL.UIRectShader.TexCoordsAttr,
                              2, 
                              GL_FLOAT, 
                              GL_FALSE,
                              4* sizeof(float), 
                              (void*)(2 * sizeof(float)));
    }
    
    glDrawElements(GL_TRIANGLES, RectBuffer->RectCount * 6, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    OpenGLDeleteHandle(&ColorsTexBuf);
    OpenGLDeleteHandle(&TypesTexBuf);
    OpenGLDeleteHandle(&TransTexBuf);
    OpenGLDeleteHandle(&IndexToTranTexBuf);
    
    glUseProgram(0);
    
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

INTERNAL_FUNCTION PLATFORM_RENDERER_RENDER(OpenGLRender)
{
    FUNCTION_TIMING();
    
    OpenGLProcessDeallocList(Commands);
    
    //glClearColor(0.3f, 0.4f, 0.8f, 1.0f);
    glClearColor(0.6f, 0.6f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glViewport(0, 0, 
               Commands->WindowDimensions.Width,
               Commands->WindowDimensions.Height);
    
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
    
    OpenGLRenderCommands(Commands);
    
    OpenGLRenderRectBuffer(Commands, &Commands->Rects3D, true, true);
    OpenGLRenderRectBuffer(Commands, &Commands->Rects2D, false, false);
}

INTERNAL_FUNCTION PLATFORM_RENDERER_SWAPBUFFERS(OpenGLSwapBuffers)
{
    SDL_GL_SwapWindow(App->Window);
}