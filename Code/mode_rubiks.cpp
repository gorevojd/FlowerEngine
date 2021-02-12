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
    
    State->Cube3 = CreateCube(Scene->Arena, 10, 1.0f, true);
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
        GenerateScrubmle(Cube, Global_Time->Time * 1000.0f);
    }
    
    if(GetKeyDown(Key_Q))
    {
        ResetCubies(Cube);
    }
    
    if(GetKeyDown(Key_R))
    {
        if(CtrlIsPressed)
        {
            CommandStandard_RR(Cube, ShiftIsPressed);
        }
        else
        {
            CommandStandard_R(Cube, ShiftIsPressed);
        }
    }
    
    if(GetKeyDown(Key_L))
    {
        if(CtrlIsPressed)
        {
            CommandStandard_LL(Cube, ShiftIsPressed);
        }
        else
        {
            CommandStandard_L(Cube, ShiftIsPressed);
        }
    }
    
    if(GetKeyDown(Key_D))
    {
        if(CtrlIsPressed)
        {
            CommandStandard_DD(Cube, ShiftIsPressed);
        }
        else
        {
            CommandStandard_D(Cube, ShiftIsPressed);
        }
    }
    
    if(GetKeyDown(Key_U))
    {
        if(CtrlIsPressed)
        {
            CommandStandard_UU(Cube, ShiftIsPressed);
        }
        else
        {
            CommandStandard_U(Cube, ShiftIsPressed);
        }
    }
    
    if(GetKeyDown(Key_F))
    {
        if(CtrlIsPressed)
        {
            CommandStandard_FF(Cube, ShiftIsPressed);
        }
        else
        {
            CommandStandard_F(Cube, ShiftIsPressed);
        }
    }
    
    if(GetKeyDown(Key_B))
    {
        if(CtrlIsPressed)
        {
            CommandStandard_BB(Cube, ShiftIsPressed);
        }
        else
        {
            CommandStandard_B(Cube, ShiftIsPressed);
        }
    }
    
    // NOTE(Dima): CenterTests
    if(GetKeyDown(Key_M) && Cube->Dim >= 3)
    {
        CommandStandard_M(Cube, ShiftIsPressed);
    }
    
    if(GetKeyDown(Key_E) && Cube->Dim >= 3)
    {
        CommandStandard_E(Cube, ShiftIsPressed);
    }
    
    if(GetKeyDown(Key_S) && Cube->Dim >= 3)
    {
        CommandStandard_S(Cube, ShiftIsPressed);
    }
    
    UpdateCube(&State->Cube3, V3(0.0f), 1.0f);
    //UpdateCube(&State->Cube3, V3(4.0f, 0.0f, 0.0f), 1.0f, true);
    ShowSides(&State->Cube3, V2(10), 240);
    
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