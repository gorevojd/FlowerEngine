inline void ChangeCameraState(game_camera* Cam, u32 State)
{
    Cam->State = State;
    
    switch(State)
    {
        case Camera_ShowcaseRotateZ:
        case Camera_RotateAround:
        {
            // NOTE(Dima): Camera rotation context
            Cam->ViewRadiusMax = 20.0f;
            Cam->ViewRadiusMin = 1.0f;
            Cam->ViewRadius = 15.0f;
            Cam->ViewTargetRadius = 7.0f;
            Cam->ShowcaseRotateTime = 12.0f;
        }break;
        
        case Camera_Follow2D:
        {
            // NOTE(Dima): Context of Follow2D camera
            Cam->WidthRadius = 2.0f;
            
            Cam->P = V3(0.0f, 0.0f, Cam->NearClipPlane + 1.0f);
            
            v3 Front = V3_Back();
            v3 Up = V3_Up();
            v3 Left = V3_Right();
            
            Cam->Transform = Matrix3FromRows(Left, Up, Front);
        }break;
    }
}

inline void InitCamera(game_camera* Cam, 
                       u32 State,
                       f32 Near = 0.5f,
                       f32 Far = 500.0f)
{
    Cam->NearClipPlane = Near;
    Cam->FarClipPlane = Far;
    
    ChangeCameraState(Cam, State);
}

// NOTE(Dima): Updating camera rotation 
void UpdateCameraRotation(game_camera* Camera,
                          float dPitch,
                          float dYaw,
                          float dRoll)
{
    float LockEdge = 85.0f;
    
    Camera->EulerAngles.Pitch += dPitch;
    Camera->EulerAngles.Yaw += dYaw;
    Camera->EulerAngles.Roll += dRoll;
    
    Camera->EulerAngles.Pitch = Clamp(Camera->EulerAngles.Pitch, -LockEdge, LockEdge);
    
    v3 Front;
    
    float Yaw = Camera->EulerAngles.Yaw * F_DEG2RAD;
    float Pitch = Camera->EulerAngles.Pitch * F_DEG2RAD;
    
    Front.x = Sin(Yaw) * Cos(Pitch);
    Front.y = Sin(Pitch);
    Front.z = Cos(Yaw) * Cos(Pitch);
    Front = NOZ(Front);
    
    v3 Left = NOZ(Cross(V3_Up(), Front));
    v3 Up = NOZ(Cross(Front, Left));
    
    Camera->Transform = Matrix3FromRows(Left, Up, Front);
}

INTERNAL_FUNCTION inline void MoveCameraToViewTarget(game_camera* Camera, f32 CamSpeed)
{
    switch(Camera->State)
    {
        case Camera_FlyAround:
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
        }break;
        
        case Camera_RotateAround:
        case Camera_ShowcaseRotateZ:
        {
            Camera->ViewTargetRadius -= Global_Input->MouseScroll * 0.4f;
            Camera->ViewTargetRadius = Clamp(Camera->ViewTargetRadius, 
                                             Camera->ViewRadiusMin, 
                                             Camera->ViewRadiusMax);
            
            Camera->ViewRadius = Lerp(Camera->ViewRadius, 
                                      Camera->ViewTargetRadius, 
                                      5.0f * Global_Time->DeltaTime);
            
            Camera->P = Camera->ViewCenterP - Camera->Transform.Rows[2] * Camera->ViewRadius;
        }break;
        
        case Camera_Follow2D:
        {
            Camera->P.xy = Lerp(Camera->P.xy, 
                                Camera->TargetP.xy, 
                                5.0f * Global_Time->DeltaTime);
        }break;
    }
}

// NOTE(Dima): Look at matrix
m44 GetViewMatrix(game_camera* Camera)
{
    m44 Result = LookAtViewMatrix(Camera->P, Camera->P + Camera->Transform.Rows[2], V3_Up());
    
    return(Result);
}

INTERNAL_FUNCTION void SetRenderPassDataFromCamera(render_pass* RenderPass,
                                                   game_camera* Camera)
{
    window_dimensions* WndDims = &Global_RenderCommands->WindowDimensions;
    
    m44 View = GetViewMatrix(Camera);
    
    b32 IsMode2D = Camera->State == Camera_Follow2D;
    
    if (IsMode2D)
    {
        f32 WidthOverHeight = (f32)WndDims->Width / (f32)WndDims->Height;
        
        SetOrthographicPassData(RenderPass,
                                Camera->P,
                                View,
                                Camera->WidthRadius,
                                Camera->WidthRadius / WidthOverHeight,
                                Camera->FarClipPlane,
                                Camera->NearClipPlane);
    }
    else
    {
        SetPerspectivePassData(RenderPass,
                               Camera->P,
                               View, 
                               WndDims->Width,
                               WndDims->Height,
                               Camera->FarClipPlane,
                               Camera->NearClipPlane);
    }
}


void UpdateCamera(game_camera* Camera, render_pass* RenderPass, f32 CamSpeed = 1.0f)
{
    f32 MouseDeltaX = 0.0f;
    f32 MouseDeltaY = 0.0f;
    
    b32 Is2DMode = Camera->State == Camera_Follow2D;
    
    if(Global_Input->CapturingMouse && !Is2DMode)
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
    MoveCameraToViewTarget(Camera, CamSpeed);
    
    SetRenderPassDataFromCamera(RenderPass, Camera);
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
                        ArrLen(Result->Name),
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
    
    if (Obj->CompModel.Free)
    {
        free(Obj->CompModel.Free);
    }
    
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
    helper_byte_buffer Help = {};
    
    game_object* Result = CreateGameObject(Game, GameObject_Object, Name);
    
    if(Model->NumNodes)
    {
        Help.AddPlace("NodeToModel", Model->NumNodes, sizeof(m44));
    }
    
    if(Model->NumBones)
    {
        Help.AddPlace("SkinningMatrices", Model->NumBones, sizeof(m44));
    }
    
    Help.Generate();
    
    component_model* ModelComp = (component_model*)&Result->CompModel;
    ModelComp->NodeToModel = (m44*)Help.GetPlace("NodeToModel");
    ModelComp->SkinningMatrices = (m44*)Help.GetPlace("SkinningMatrices");
    ModelComp->Free = Help.Data;
    ModelComp->Model = Model;
    ModelComp->ToModelIsComputed = false;
    
    return(Result);
}

void UpdateModelGameObject(game_object* Object)
{
    component_model* CompModel = (component_model*)&Object->CompModel;
    
    m44* SkinningMatrices = 0;
    int SkinningMatricesCount = 0;
    model* Model = CompModel->Model;
    
    component_animator* AnimatorComp = (component_animator*)&Object->CompAnimator;
    animation* Animation = 0;
    
    if(AnimatorComp)
    {
        Animation = AnimatorComp->PlayingAnimation;
    }
    
    if(Animation != 0)
    {
        UpdateAnimation(Animation, Global_Time->Time, Model->Node_ToParent);
        
        SkinningMatrices = CompModel->SkinningMatrices;
        SkinningMatricesCount = Model->NumBones;
        
        CalculateToModelTransforms(Model, CompModel->NodeToModel);
        // NOTE(Dima): Setting this to false so next time they should be recomputed again
        CompModel->ToModelIsComputed = false;
        
        CalculateSkinningMatrices(Model,
                                  CompModel->NodeToModel,
                                  CompModel->SkinningMatrices);
    }
    else
    {
        if(!CompModel->ToModelIsComputed)
        {
            CalculateToModelTransforms(Model, CompModel->NodeToModel);
            // NOTE(Dima): 
            CompModel->ToModelIsComputed = true;
        }
    }
    
    m44 ModelToWorld = TranslationMatrix(Object->P);
    
    
    for(int NodeIndex = 0;
        NodeIndex < Model->NumNodes;
        NodeIndex++)
    {
        model_node* Node = &Model->Nodes[NodeIndex];
        
        m44 NodeTran = CompModel->NodeToModel[NodeIndex] * ModelToWorld;
        
        for(int MeshIndex = 0;
            MeshIndex < Node->NumMeshIndices;
            MeshIndex++)
        {
            int ActualMeshIndex = Model->NodesMeshIndices[Node->StartInMeshIndices + MeshIndex];
            
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

#define PROCESS_GAME_OBJECT_FUNC(name) void name(game* Game, game_object* Obj)
typedef PROCESS_GAME_OBJECT_FUNC(process_game_object_func);

PROCESS_GAME_OBJECT_FUNC(UpdateGameObject)
{
    switch(Obj->Type)
    {
        case GameObject_Object:
        {
            UpdateModelGameObject(Obj);
        }break;
        
        default:
        {
            // NOTE(Dima): Nothing to do!
        }break;
    }
}

PROCESS_GAME_OBJECT_FUNC(RenderGameObject)
{
    switch(Obj->Type)
    {
        case GameObject_Object:
        {
            // TODO(Dima): Do something
        }break;
        
        default:
        {
            // NOTE(Dima): Nothing to do!
        }break;
    }
}

void GameObjectRec(game* Game, game_object* Obj, process_game_object_func* Func)
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
                GameObjectRec(Game, At, Func);
                
                At = At->Next;
            }
        }
        
        Func(Game, Obj);
    }
}

void UpdateGameObjects(game* Game)
{
    GameObjectRec(Game, Game->GameObjectPool.Root, UpdateGameObject);
}
