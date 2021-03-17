enum test_game_mode
{
    TestGame_Animals,
    TestGame_CubeField,
    
    TestGame_Count,
};

struct testgame_state
{
    game_camera Camera;
    
    u32 Mode;
    
    game_object* Bear1;
    game_object* Bear2;
    game_object* Fox;
    game_object* Supra;
};

SCENE_INIT(TestGame)
{
    testgame_state* State = GetSceneState(testgame_state);
    
    State->Camera.P = V3(0.0f, 0.0f, 5.0f);
    quat InitRot = LookRotation(V3_Back(), V3_Up());
    State->Camera.EulerAngles = QuatToEuler(InitRot);
    
    InitCamera(&State->Camera, Camera_FlyAround);
    
    State->Bear1 = CreateModelGameObject(Scene->Game, &Global_Assets->Bear);
    State->Bear2 = CreateModelGameObject(Scene->Game, &Global_Assets->Bear);
    State->Fox = CreateModelGameObject(Scene->Game, &Global_Assets->Fox);
    State->Supra = CreateModelGameObject(Scene->Game, &Global_Assets->Supra);
    
    AddComponent(State->Bear1, Component_component_animator);
    AddComponent(State->Bear2, Component_component_animator);
    AddComponent(State->Fox, Component_component_animator);
    
    State->Bear1->P = V3(0.0f, 0.0f, 0.0f);
    GetComp(State->Bear1, component_animator)->PlayingAnimation = &Global_Assets->BearSuccess;
    
    State->Bear2->P = V3(2.0f, 0.0f, 0.0f);
    GetComp(State->Bear2, component_animator)->PlayingAnimation = &Global_Assets->BearIdle;
    
    State->Fox->P = V3(4.0f, 0.0f, 0.0f);
    GetComp(State->Fox, component_animator)->PlayingAnimation = &Global_Assets->FoxTalk;
    
    State->Supra->P = V3(6.0f, 0.0f, 0.0f);
}

SCENE_UPDATE(TestGame)
{
    testgame_state* State = GetSceneState(testgame_state);
    
    // NOTE(Dima): Speed multiplyer
    float SpeedMultiplier = 5.0f;
    if(GetKey(Key_LeftShift))
    {
        SpeedMultiplier *= 5.0f;
    }
    if(GetKey(Key_Space))
    {
        SpeedMultiplier *= 5.0f;
    }
    
    // NOTE(Dima): Updating camera
    UpdateCamera(&State->Camera, SpeedMultiplier);
    
    // NOTE(Dima): Updating game mode
    if(GetKeyDown(Key_M))
    {
        State->Mode = (State->Mode + 1) % TestGame_Count;
    }
    
    switch(State->Mode)
    {
        case TestGame_Animals:
        {
            
            
            PushMesh(&Global_Assets->Plane,
                     &Global_Assets->GroundMaterial,
                     ScalingMatrix(10.0f),
                     V3(1.0f));
        }break;
        
        case TestGame_CubeField:
        {
            PushMesh(&Global_Assets->Plane,
                     0,
                     ScalingMatrix(20.0f),
                     V3(0.7f));
            
            float CubeSpacing = 1.5f;
            
            v4 Color00 = ColorFromHex("#29FFC9");
            v4 Color01 = ColorFromHex("#7E42F5");
            v4 Color10 = ColorFromHex("#FF5114");
            v4 Color11 = ColorRed();
            
            int SideLen = 10;
            f32 DimCubes = CubeSpacing * (f32)SideLen;
            
            v3 CubeAddOffset = V3(-DimCubes, 0.0f, -DimCubes) * 0.5f;
            
            for(int y = 0; y < SideLen; y++)
            {
                for(int x = 0; x < SideLen; x++)
                {
                    float HorzPercentage = (float)x / (float)(SideLen - 1);
                    float VertPercentage = (float)y / (float)(SideLen - 1);
                    
                    v3 ColorHorzBottom = Lerp(Color00, Color10, HorzPercentage).rgb;
                    v3 ColorHorzTop = Lerp(Color01, Color11, HorzPercentage).rgb;
                    v3 VertColor = Lerp(ColorHorzBottom, ColorHorzTop, VertPercentage);
                    
                    v3 P = V3((f32)x * CubeSpacing, 
                              0.5f, 
                              (f32)y * CubeSpacing) + CubeAddOffset;
                    
                    PushMesh(&Global_Assets->Cube, 
                             0,
                             TranslationMatrix(P), 
                             VertColor);
                }
            }
        }break;
    }
    
    
    UpdateGameObjects(Scene->Game);
    
    SetMatrices(&State->Camera);
    
#if 1
    // NOTE(Dima): SSAO samples
    for(int i = 0; i < Global_RenderCommands->PostProcessing.SSAO_Params.KernelSize; i++)
    {
        v3 Sample = Global_RenderCommands->PostProcessing.SSAO_Kernel[i];
        
        v3 Pos = Sample * 3.0f;
        PushMesh(&Global_Assets->Cube,
                 0,
                 ScalingMatrix(0.05f) * TranslationMatrix(Pos));
    }
#endif
    
}


SCENE_ONGUI(TestGame)
{
    
}