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
    game_object* Mustang;
    game_object* NissanGTR;
    game_object* Golf2;
    game_object* Aventador;
};

SCENE_INIT(TestGame)
{
    testgame_state* State = GetSceneState(testgame_state);
    
    State->Camera.P = V3(0.0f, 0.0f, 5.0f);
    quat InitRot = LookRotation(V3_Back(), V3_Up());
    State->Camera.EulerAngles = QuatToEuler(InitRot);
    
    InitCamera(&State->Camera, Camera_FlyAround, 0.5f, 500.0f);
    
    // NOTE(Dima): Cars
    asset_id SupraID = GetAssetID("Model_Car_Supra");
    asset_id MustangID = GetAssetID("Model_Car_Mustang");
    asset_id NissanGTRID = GetAssetID("Model_Car_NissanGTR");
    asset_id Golf2ID = GetAssetID("Model_Car_Golf2");
    asset_id AventadorID = GetAssetID("Model_Car_LamborginiAventador");
    
    State->Supra = CreateModelGameObject(Scene->Game, G_GetAssetDataByID(SupraID, model));
    State->Mustang = CreateModelGameObject(Scene->Game, G_GetAssetDataByID(MustangID, model));
    State->NissanGTR = CreateModelGameObject(Scene->Game, G_GetAssetDataByID(NissanGTRID, model));
    State->Golf2 = CreateModelGameObject(Scene->Game, G_GetAssetDataByID(Golf2ID, model));
    State->Aventador = CreateModelGameObject(Scene->Game, G_GetAssetDataByID(AventadorID, model));
    
    State->Supra->P = V3(6.0f, 0.0f, 0.0f);
    State->Mustang->P = V3(10.0f, 0.0f, 0.0f);
    State->NissanGTR->P = V3_Left() * 14.0f;
    State->Golf2->P = V3_Left() * 18.0f;
    State->Aventador->P = V3_Left() * 22.0f;
    
    
    // NOTE(Dima): Animals
    asset_id ModelBearID = GetAssetID("Model_Bear");
    asset_id ModelFoxID = GetAssetID("Model_Fox");
    
    State->Bear1 = CreateModelGameObject(Scene->Game, G_GetAssetDataByID(ModelBearID, model));
    State->Bear2 = CreateModelGameObject(Scene->Game, G_GetAssetDataByID(ModelBearID, model));
    State->Fox = CreateModelGameObject(Scene->Game, G_GetAssetDataByID(ModelFoxID, model));
    
    
    State->Bear1->P = V3(0.0f, 0.0f, 0.0f);
    State->Bear2->P = V3(2.0f, 0.0f, 0.0f);
    State->Fox->P = V3(4.0f, 0.0f, 0.0f);
    
#if 1    
    asset_id BearSuccessID = GetAssetID("Anim_Bear_Success");
    asset_id BearIdleID = GetAssetID("Anim_Bear_Idle");
    asset_id FoxTalkID = GetAssetID("Anim_Fox_Talk");
    
    State->Bear1->CompAnimator.PlayingAnimation = G_GetAssetDataByID(BearSuccessID, 
                                                                     animation);
    
    State->Bear2->CompAnimator.PlayingAnimation = G_GetAssetDataByID(BearIdleID, 
                                                                     animation);
    
    State->Fox->CompAnimator.PlayingAnimation = G_GetAssetDataByID(FoxTalkID, 
                                                                   animation);
#endif
    
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
    
    
    render_pass* RenderPass = AddRenderPass();
    
    // NOTE(Dima): Updating camera
    UpdateCamera(&State->Camera, RenderPass, SpeedMultiplier);
    
    
    // NOTE(Dima): Updating game mode
    if(GetKeyDown(Key_M))
    {
        State->Mode = (State->Mode + 1) % TestGame_Count;
    }
    
    switch(State->Mode)
    {
        case TestGame_Animals:
        {
            
            m44 PlaneMatrix = ScalingMatrix(V3(30.0f)) * 
                TranslationMatrix(V3(12.0f, 0.0f, 0.0f));
            
#if 0            
            PushMesh(GetGlobalAssetStorage(),
                     true,
                     GetAssetID("Mesh_Plane"),
                     GetAssetID("Material_DefaultPlane"),
                     PlaneMatrix,
                     V3(1.0f));
#else
            PushMesh(GetGlobalAssetStorage(),
                     true, 
                     GetAssetID("Mesh_Plane"),
                     GetAssetID("Material_DefaultPlane"),
                     PlaneMatrix, 
                     V3(0.6f));
#endif
            
            
            // NOTE(Dima): Updating cubes
            v3 RotAxis = NOZ(V3(Cos(Global_Time->Time),
                                Sin(Global_Time->Time * 1.1f),
                                Cos(Global_Time->Time * 1.05f + 3123.0f)));
            
            quat Rot = AxisAngle(RotAxis, 0.0f);
            
            PushMesh(GetGlobalAssetStorage(),
                     true,
                     GetAssetID("Mesh_Cube"), 
                     GetAssetID("Material_DefaultCube"),
                     QuaternionToMatrix4(Rot) * TranslationMatrix(V3(-2.5f, 0.45f, 0.0f)));
            
            v3 Cube2P = V3(-4.0f, 
                           Cos(Global_Time->Time) * 0.9f,
                           0.0f);
            
            PushMesh(GetGlobalAssetStorage(),
                     true,
                     GetAssetID("Mesh_Cube"),
                     GetAssetID("Material_DefaultCube"),
                     QuaternionToMatrix4(Rot) * TranslationMatrix(Cube2P));
            
            v3 Cube3P = V3(-5.5f, 0.45f, 0.0f);
            PushMesh(GetGlobalAssetStorage(),
                     true,
                     GetAssetID("Mesh_Cube"),
                     0,
                     LookRotationMatrix(Cube2P - Cube3P) * TranslationMatrix(Cube3P),
                     V3(1.0f, 0.25f, 0.1f));
            
        }break;
        
        case TestGame_CubeField:
        {
            PushMesh(GetGlobalAssetStorage(),
                     GetAssetID("Mesh_Plane"),
                     GetAssetID("Material_DefaultPlane"),
                     true, 
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
            
            asset_id MeshAssetID = GetAssetID("Mesh_Cube");
            asset_id MaterialAssetID = GetAssetID("Material_DefaultCube");
            
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
                    
                    PushMesh(GetGlobalAssetStorage(),
                             true,
                             MeshAssetID,
                             MaterialAssetID,
                             TranslationMatrix(P), 
                             VertColor);
                }
            }
        }break;
    }
    
    PushClear(V3(1.0f, 0.0f, 1.0f));
    PushSky(GetGlobalAssetStorage(),
            true,
            GetAssetID("Cubemap_DefaultPink"));
    
    UpdateGameObjects(Scene->Game);
    
    directional_light* DirLit = &Global_RenderCommands->Lighting.DirLit;
    
#if 0    
    DirLit->Dir = Lerp(NOZ(V3(-0.5f, -0.5f, -10.8f)), 
                       NOZ(V3(-0.5f, -0.5f, -0.8f)), 
                       Cos(Global_Time->Time) * 0.5f + 0.5f);
#else
    DirLit->Dir = NOZ(V3(-0.5f, -0.5f, -0.8f)); 
#endif
    
    UpdateShadowCascades(RenderPass);
    
#if 0
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
    
#if 1    
    render_pass* OrthoPass = AddRenderPass();
    SetOrthographicPassData(OrthoPass,
                            V3(0.0f),
                            IdentityMatrix4(),
                            Global_RenderCommands->WindowDimensions.Width,
                            Global_RenderCommands->WindowDimensions.Height,
                            500, 0.1f);
    
    RectBufferSetMatrices(Global_RenderCommands->DEBUG_Rects2D_Unit,
                          IdentityMatrix4(),
                          OrthographicProjectionUnit(Global_RenderCommands->WindowDimensions.Width,
                                                     Global_RenderCommands->WindowDimensions.Height));
    
    f32 RectAtX = -1.0f;
    for (int i = 0; i < 10; i++)
    {
        PushRect(Global_RenderCommands->DEBUG_Rects2D_Unit, 
                 RectMinDim(V2(RectAtX, 1.5f), V2(0.1f)),
                 ColorRed());
        
        RectAtX += 0.2f;
    }
#endif
    
    asset_id BerlinSansID = GetAssetID("Font_BerlinSans");
    asset_id LiberationMonoID = GetAssetID("Font_LiberationMono");
    
    font* BerlinSans = G_GetAssetDataByID(BerlinSansID, font);
    font* LiberationMono = G_GetAssetDataByID(LiberationMonoID, font);
    
    PushImage(BerlinSans->Atlas, V2(100), 1024);
    PrintTextWithFont(BerlinSans,
                      "Hello world. My Name is Dima",
                      V2(1000, 100),
                      25.0f,
                      ColorRed());
    PrintTextWithFont(LiberationMono,
                      "And I love programming",
                      V2(1000, 300),
                      40.0f,
                      ColorGreen());
    
    PrintTextWithFont(LiberationMono,
                      "And Making 123 Videos for YouTube",
                      V2(100, 600),
                      100.0f + Sin(Global_Time->Time) * 60.0f,
                      ColorGreen());
    
}


SCENE_ONGUI(TestGame)
{
    
}