inline void InitCamera(game_camera* Cam, u32 State)
{
    Cam->State = State;
    
    
    Cam->ViewRadiusMax = 20.0f;
    Cam->ViewRadiusMin = 1.0f;
    Cam->ViewRadius = 15.0f;
    Cam->ViewTargetRadius = 7.0f;
    Cam->ShowcaseRotateTime = 12.0f;
}

// NOTE(Dima): Updating camera rotation 
void UpdateCameraRotation(game_camera* Camera,
                          float dPitch,
                          float dYaw,
                          float dRoll)
{
    float LockEdge = 89.0f;
    
    Camera->EulerAngles.Pitch += dPitch;
    Camera->EulerAngles.Yaw += dYaw;
    Camera->EulerAngles.Roll += dRoll;
    
    Camera->EulerAngles.Pitch = Clamp(Camera->EulerAngles.Pitch, -LockEdge, LockEdge);
    
    v3 Front;
    
    float Yaw = Camera->EulerAngles.Yaw * F_DEG2RAD;
    float Pitch = Camera->EulerAngles.Pitch * F_DEG2RAD;
    
#if 1
    Front.x = Sin(Yaw) * Cos(Pitch);
    Front.y = Sin(Pitch);
    Front.z = Cos(Yaw) * Cos(Pitch);
    Front = NOZ(Front);
    
    v3 Left = NOZ(Cross(V3_Up(), Front));
    v3 Up = NOZ(Cross(Front, Left));
    
    Camera->Transform = Matrix3FromRows(Left, Up, Front);
#else
    
    m44 RotYaw = RotationMatrixY(Yaw);
    m44 RotPitch = RotationMatrixX(Pitch);
    
    Camera->Transform = Matrix4ToMatrix3(RotPitch * RotYaw);
#endif
}

INTERNAL_FUNCTION inline void MoveCameraToViewTarget(game_camera* Camera)
{
    Camera->ViewTargetRadius -= Global_Input->MouseScroll * 0.4f;
    Camera->ViewTargetRadius = Clamp(Camera->ViewTargetRadius, 
                                     Camera->ViewRadiusMin, 
                                     Camera->ViewRadiusMax);
    
    Camera->ViewRadius = Lerp(Camera->ViewRadius, Camera->ViewTargetRadius, 5.0f * Global_Time->DeltaTime);
}

void UpdateCamera(game_camera* Camera, f32 CamSpeed = 1.0f)
{
    f32 MouseDeltaX = 0.0f;
    f32 MouseDeltaY = 0.0f;
    
    if(Global_Input->CapturingMouse)
    {
        MouseDeltaX = GetAxis(Axis_MouseX);
        MouseDeltaY = GetAxis(Axis_MouseY);
        
        if(Camera->State == Camera_ShowcaseRotateZ)
        {
            MouseDeltaX = Global_Time->DeltaTime * F_RAD2DEG * F_TWO_PI / Camera->ShowcaseRotateTime;
            MouseDeltaY = 0.0f;
        }
    }
    
    if(Camera->State == Camera_ShowcaseRotateZ)
    {
        MouseDeltaX = Global_Time->DeltaTime * F_RAD2DEG * F_TWO_PI / Camera->ShowcaseRotateTime;
    }
    
    UpdateCameraRotation(Camera, MouseDeltaY, MouseDeltaX, 0.0f);
    
    if(Camera->State == Camera_FlyAround)
    {
        f32 HorzMove = GetAxis(Axis_Horizontal);
        f32 VertMove = GetAxis(Axis_Vertical);
        
        v3 Moves = NOZ(V3(HorzMove, 0.0f, VertMove));
        
        v3 MoveVector = Moves * Global_Time->DeltaTime;
        
        v3 TransformedMoveVector = CamSpeed * MoveVector * Camera->Transform;
        
        if(Global_Input->CapturingMouse)
        {
            Camera->P += TransformedMoveVector;
        }
    }
    else if(Camera->State == Camera_RotateAround)
    {
        MoveCameraToViewTarget(Camera);
        
        v3 CamFront = Camera->Transform.Rows[2];
        
        Camera->P = Camera->ViewCenterP - CamFront * Camera->ViewRadius;
    }
    else if(Camera->State == Camera_ShowcaseRotateZ)
    {
        MoveCameraToViewTarget(Camera);
        
        v3 CamFront = Camera->Transform.Rows[2];
        
        Camera->P = Camera->ViewCenterP - CamFront * Camera->ViewRadius;
    }
};

// NOTE(Dima): Look at matrix
m44 GetViewMatrix(game_camera* Camera)
{
    m44 Result = LookAt(Camera->P, Camera->P + Camera->Transform.Rows[2], V3_Up());
    
    return(Result);
}

void ShowLabel3D(game_camera* Camera,
                 char* Text,
                 v3 P,
                 f32 UnitHeight = 0.2f,
                 v4 Color = ColorWhite())
{
    v3 Normal = -Camera->Transform.Rows[2];
    
    PrintTextCentered3D(Text, P,
                        Normal,
                        UnitHeight,
                        Color);
}

INTERNAL_FUNCTION void SetMatrices(m44 ViewMatrix)
{
    window_dimensions* WndDims = &Global_RenderCommands->WindowDimensions;
    
    Global_RenderCommands->View = ViewMatrix;
    Global_RenderCommands->Projection = PerspectiveProjection(WndDims->Width, 
                                                              WndDims->Height,
                                                              500.0f, 0.5f);
    
    Global_RenderCommands->ViewProjection = 
        Global_RenderCommands->View * 
        Global_RenderCommands->Projection;
}

// NOTE(Dima): Object pool stuff
inline game_object* AllocateGameObject(game* Game)
{
    game_object_pool* Pool = &Game->GameObjectPool;
    
    if(DLIST_FREE_IS_EMPTY(Pool->FreeSentinel, NextAlloc))
    {
        int CountNewObjects = 512;
        
        game_object* NewObjects = PushArray(Game->Arena, game_object, CountNewObjects);
        
        for(int ObjectIndex = 0;
            ObjectIndex < CountNewObjects;
            ObjectIndex++)
        {
            game_object* CurrentObj = NewObjects + ObjectIndex;
            
            CurrentObj->CreationIndex = Pool->CreatedObjectsCount + ObjectIndex;
            
            DLIST_INSERT_BEFORE_SENTINEL(CurrentObj, Pool->FreeSentinel, 
                                         NextAlloc, PrevAlloc);
        }
    }
    
    game_object* Result = Pool->FreeSentinel.NextAlloc;
    
    // NOTE(Dima): Removing from Free & inserting to Use list
    DLIST_REMOVE(Result, NextAlloc, PrevAlloc);
    DLIST_INSERT_BEFORE_SENTINEL(Result, Pool->UseSentinel,
                                 NextAlloc, PrevAlloc);
    
    return(Result);
}

inline void DeallocateGameObject(game* Game, game_object* Object)
{
    game_object_pool* Pool = &Game->GameObjectPool;
    
    // NOTE(Dima): Removing from Use & inserting to Free list
    DLIST_REMOVE(Object, NextAlloc, PrevAlloc);
    DLIST_INSERT_BEFORE_SENTINEL(Object, Pool->FreeSentinel,
                                 NextAlloc, PrevAlloc);
}

game_object* CreateGameObject(game* Game, u32 Type, 
                              char* Name = 0, 
                              game_object* Parent = 0,
                              b32 IsActive = true)
{
    game_object_pool* Pool = &Game->GameObjectPool;
    
    // NOTE(Dima): Allocating
    game_object* Result = AllocateGameObject(Game);
    Result->Type = Type;
    
    // NOTE(Dima): Init object parent
    Result->IsActive = IsActive;
    Result->Parent = Pool->Root;
    if(Parent)
    {
        Result->Parent = Parent;
    }
    
    // NOTE(Dima): Init ptrs
    if(Type == GameObject_Sentinel ||
       Type == GameObject_Root)
    {
        DLIST_REFLECT_POINTER_PTRS(Result, Next, Prev);
    }
    else
    {
        DLIST_INSERT_BEFORE(Result, Result->Parent->ChildSentinel, Next, Prev);
    }
    
    // NOTE(Dima): Init Child sentinels
    if(Type == GameObject_Sentinel)
    {
        Result->ChildSentinel = 0;
    }
    else
    {
        // NOTE(Dima): Creating child sentinel
        Result->ChildSentinel = CreateGameObject(Game, 
                                                 GameObject_Sentinel,
                                                 "Sentinel",
                                                 Result, 
                                                 false);
    }
    
    // NOTE(Dima): Init object name
    if(!Name)
    {
        stbsp_sprintf(Result->Name, "Object %d", Result->CreationIndex);
    }
    else
    {
        CopyStringsSafe(Result->Name, 
                        ARC(Result->Name),
                        Name);
    }
    
    // NOTE(Dima): Init transforms
    Result->P = V3_Zero();
    Result->R = IdentityQuaternion();
    Result->S = V3_One();
    
    return(Result);
}

void DeleteGameObjectRec(game* Game, game_object* Obj)
{
    if(Obj->ChildSentinel)
    {
        game_object* At = Obj->ChildSentinel->Next;
        while(At != Obj->ChildSentinel)
        {
            DeleteGameObjectRec(Game, At);
            
            At = At->Next;
        }
        
        DLIST_REMOVE(Obj->ChildSentinel, Next, Prev);
        DeallocateGameObject(Game, Obj->ChildSentinel);
    }
    
    // TODO(Dima): If needed - we can call destructors on object's components here
    // ....
    
    // NOTE(Dima): Deleting this object
    DLIST_REMOVE(Obj, Next, Prev);
    DeallocateGameObject(Game, Obj);
}

void DeleteGameObject(game* Game, game_object* Obj)
{
    DeleteGameObjectRec(Game, Obj);
}

void InitGameObjectPool(game* Game,
                        memory_arena* Arena)
{
    game_object_pool* Pool = &Game->GameObjectPool;
    
    DLIST_REFLECT_PTRS(Pool->UseSentinel, NextAlloc, PrevAlloc);
    DLIST_REFLECT_PTRS(Pool->FreeSentinel, NextAlloc, PrevAlloc);
    
    Pool->CreatedObjectsCount = 0;
    
    // NOTE(Dima): Init root object and it's sentinel
    Pool->Root = CreateGameObject(Game, GameObject_Root, "Root");
}

game_object* CreateModelGameObject(game* Game,
                                   model* Model,
                                   char* Name = 0)
{
    game_object* Result = CreateGameObject(Game, GameObject_Model, Name);
    
    Result->Model_Model = Model;
    
    if(Model->Shared.NumNodes)
    {
        Result->Model_NodeToModel = PushArray(Game->Arena, m44, Model->Shared.NumNodes);
    }
    
    if(Model->Shared.NumBones)
    {
        Result->Model_SkinningMatrices = PushArray(Game->Arena, m44, Model->Shared.NumBones);
    }
    
    return(Result);
}

void UpdateModelGameObject(game_object* Object)
{
    m44* SkinningMatrices = 0;
    int SkinningMatricesCount = 0;
    model* Model = Object->Model_Model;
    animation* Animation = Object->Model_PlayingAnimation;
    
    if(Animation != 0)
    {
        UpdateAnimation(Animation, Global_Time->Time, Model->Node_ToParent);
        
        SkinningMatrices = Object->Model_SkinningMatrices;
        SkinningMatricesCount = Model->Shared.NumBones;
        
        CalculateToModelTransforms(Model, Object->Model_NodeToModel);
        // NOTE(Dima): Setting this to false so next time they should be recomputed again
        Object->Model_ToModelIsComputed = false;
        
        CalculateSkinningMatrices(Model,
                                  Object->Model_NodeToModel,
                                  Object->Model_SkinningMatrices);
    }
    else
    {
        if(!Object->Model_ToModelIsComputed)
        {
            CalculateToModelTransforms(Model, Object->Model_NodeToModel);
            // NOTE(Dima): 
            Object->Model_ToModelIsComputed = true;
        }
    }
    
    m44 ModelToWorld = TranslationMatrix(Object->P);
    
    for(int NodeIndex = 0;
        NodeIndex < Model->Shared.NumNodes;
        NodeIndex++)
    {
        model_node* Node = &Model->Nodes[NodeIndex];
        
        m44 NodeTran = Object->Model_NodeToModel[NodeIndex] * ModelToWorld;
        
        for(int MeshIndex = 0;
            MeshIndex < Node->NumMeshIndices;
            MeshIndex++)
        {
            int ActualMeshIndex = Node->MeshIndices[MeshIndex];
            
            mesh* Mesh = Model->Meshes[ActualMeshIndex];
            
            if(Mesh)
            {
                m44 MeshTran = NodeTran;
                if(Mesh->IsSkinned)
                {
                    MeshTran = ModelToWorld;
                }
                
                material* Material = Model->Materials[Mesh->MaterialIndexInModel];
                
                if(Mesh->IsSkinned)
                {
                    PushMesh(Mesh, Material, MeshTran,
                             V3(1.0f),
                             SkinningMatrices,
                             SkinningMatricesCount);
                }
                else
                {
                    PushMesh(Mesh, Material, MeshTran);
                }
            }
        }
    }
}

void UpdateGameObjectRec(game* Game, game_object* Obj)
{
    Assert(Obj->ChildSentinel);
    
    if(Obj->IsActive)
    {
        // NOTE(Dima): Iterating over children and update them recursively
        if(Obj->ChildSentinel)
        {
            game_object* At = Obj->ChildSentinel->Next;
            
            while(At != Obj->ChildSentinel)
            {
                UpdateGameObjectRec(Game, At);
                
                At = At->Next;
            }
        }
        
        switch(Obj->Type)
        {
            case GameObject_Model:
            {
                UpdateModelGameObject(Obj);
            }break;
            
            default:
            {
                // NOTE(Dima): Nothing to do!
            }break;
        }
    }
}

void UpdateGameObjects(game* Game)
{
    UpdateGameObjectRec(Game, Game->GameObjectPool.Root);
}
