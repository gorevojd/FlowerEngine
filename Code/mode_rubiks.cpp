#include "flower_rubiks.cpp"

struct rubiks_state
{
    game_camera Camera;
    
    rubiks_cube Cube3;
};

SCENE_INIT(RubiksCube)
{
    rubiks_state* State = GetSceneState(rubiks_state);
    
    InitCamera(&State->Camera, Camera_RotateAround);
    
    State->Cube3 = CreateCube(Scene->Arena, 5, 1.0f, true);
}

SCENE_UPDATE(RubiksCube)
{
    rubiks_state* State = GetSceneState(rubiks_state);
    
    u32 CameraStates[2] = 
    {
        Camera_RotateAround,
        Camera_ShowcaseRotateZ,
    };
    
    LOCAL_PERSIST b32 CameraBehaviourIndex = 0;
    if(GetKeyDown(Key_V))
    {
        CameraBehaviourIndex = !CameraBehaviourIndex;
    }
    State->Camera.State = CameraStates[CameraBehaviourIndex];
    
    UpdateCamera(&State->Camera);
    
    rubiks_cube* Cube = &State->Cube3;
    
    b32 ShiftIsPressed = GetKey(Key_LeftShift);
    b32 CtrlIsPressed = GetKey(Key_LeftControl);
    
    if(GetKeyDown(Key_G))
    {
        GenerateScrubmle(Cube, Global_Time->Time * 100000.0f);
        Cube->SolvingState = RubState_Disassembled;
    }
    
    if(GetKeyDown(Key_Q))
    {
        ResetCubies(Cube);
    }
    
    if(CtrlIsPressed)
    {
        if(GetKeyDown(Key_X))
        {
            RubCom_X(Cube, ShiftIsPressed);
        }
        
        if(GetKeyDown(Key_Y))
        {
            RubCom_Y(Cube, ShiftIsPressed);
        }
        
        if(GetKeyDown(Key_Z))
        {
            RubCom_Z(Cube, ShiftIsPressed);
        }
    }
    
    if(GetKeyDown(Key_R))
    {
        if(CtrlIsPressed)
        {
            RubCom_RR(Cube, ShiftIsPressed);
        }
        else
        {
            RubCom_R(Cube, ShiftIsPressed);
        }
    }
    
    if(GetKeyDown(Key_L))
    {
        if(CtrlIsPressed)
        {
            RubCom_LL(Cube, ShiftIsPressed);
        }
        else
        {
            RubCom_L(Cube, ShiftIsPressed);
        }
    }
    
    if(GetKeyDown(Key_D))
    {
        if(CtrlIsPressed)
        {
            RubCom_DD(Cube, ShiftIsPressed);
        }
        else
        {
            RubCom_D(Cube, ShiftIsPressed);
        }
    }
    
    if(GetKeyDown(Key_U))
    {
        if(CtrlIsPressed)
        {
            RubCom_UU(Cube, ShiftIsPressed);
        }
        else
        {
            RubCom_U(Cube, ShiftIsPressed);
        }
    }
    
    if(GetKeyDown(Key_F))
    {
        if(CtrlIsPressed)
        {
            RubCom_FF(Cube, ShiftIsPressed);
        }
        else
        {
            RubCom_F(Cube, ShiftIsPressed);
        }
    }
    
    if(GetKeyDown(Key_B))
    {
        if(CtrlIsPressed)
        {
            RubCom_BB(Cube, ShiftIsPressed);
        }
        else
        {
            RubCom_B(Cube, ShiftIsPressed);
        }
    }
    
    // NOTE(Dima): CenterTests
    if(GetKeyDown(Key_M) && Cube->Dim >= 3)
    {
        RubCom_M(Cube, ShiftIsPressed);
    }
    
    if(GetKeyDown(Key_E) && Cube->Dim >= 3)
    {
        RubCom_E(Cube, ShiftIsPressed);
    }
    
    if(GetKeyDown(Key_S) && Cube->Dim >= 3)
    {
        RubCom_S(Cube, ShiftIsPressed);
    }
    
    // NOTE(Dima): StartSolve
    if(GetKeyDown(Key_Return))
    {
        if(!CubeIsSolved(Cube))
        {
            Cube->SolvingState = RubState_SolvingCenters;
        }
    }
    
    static b32 DebugMode = false;
    if(GetKeyDown(Key_Space))
    {
        DebugMode = !DebugMode;
    }
    UpdateCube(&State->Cube3, V3(0.0f), 1.0f, DebugMode);
    //UpdateCube(&State->Cube3, V3(4.0f, 0.0f, 0.0f), 1.0f, true);
    
    
#if 0    
    ShowSides(&State->Cube3, V2(10), 240);
    // NOTE(Dima): Helper left cubie
    PushMesh(&Global_Assets->Cube,
             0,
             ScalingMatrix(0.1f) * TranslationMatrix(V3(2.0f, 0.0f, 0.0f)));
#endif
    
#if 0    
    ShowLabel3D(&State->Camera, 
                "Hello world this is the cube",
                V3_Up() * 1.5f,
                0.25f,
                ColorRed());
#endif
    
#if 0    
    PushImage(&Global_Assets->FontsAtlas, V2(0.0f), 1300);
#endif
    
    SetMatrices(GetViewMatrix(&State->Camera));
}