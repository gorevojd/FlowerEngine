#include "flower.h"

platform_api PlatformAPI;
renderer_api RenderAPI;
GLOBAL_VARIABLE input_system* Global_Input;
GLOBAL_VARIABLE time_system* Global_Time;
GLOBAL_VARIABLE asset_system* Global_Assets;
GLOBAL_VARIABLE ui_state* Global_UI;
GLOBAL_VARIABLE render_commands* Global_RenderCommands;
GLOBAL_VARIABLE job_system* Global_Jobs;

#if defined(INTERNAL_BUILD)
GLOBAL_VARIABLE debug_state* Global_Debug;
debug_global_table* Global_DebugTable;
#endif

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

inline void OutputLog(const char* Format, ...)
{
    char Buf[256];
    
    va_list va;
    va_start(va, Format);
    stbsp_vsnprintf(Buf, ArrLen(Buf), Format, va);
    va_end(va);
    
    PlatformAPI.Log(Buf);
}

#include "flower_standard.cpp"
#include "flower_jobs.cpp"
#include "flower_random.cpp"
#include "flower_input.cpp"

// TODO(Dima): Maybe delete this here when asset loader implemented
// TODO(Dima): Because this file only contains information that is needed in the asset loader(packer).
#include "flower_asset_load.cpp"

#include "flower_asset.cpp"
#include "flower_render.cpp"
#include "flower_render_asset.cpp"
#include "flower_ui.cpp"
#include "flower_animation.cpp"
#include "flower_gameplay.cpp"
#include "flower_debug.cpp"

#include "flower_scene.cpp"
#include "game/meta_scenes.cpp"

INTERNAL_FUNCTION inline void InitSceneMethods(game* Game, int SceneIndex)
{
    scene* Scene = &Game->Scenes[SceneIndex];
    
    Scene->Init = MetaScene_InitFunctions[SceneIndex];
    Scene->Update = MetaScene_UpdateFunctions[SceneIndex];
    Scene->OnGUI = MetaScene_OnGUIFunctions[SceneIndex];
}

INTERNAL_FUNCTION inline void InitScene(game* Game, int SceneIndex)
{
    scene* Scene = &Game->Scenes[SceneIndex];
    
    CopyStringsSafe(Scene->Name, ArrLen(Scene->Name), (char*)MetaScene_Names[SceneIndex]);
    
    InitSceneMethods(Game, SceneIndex);
    
    Scene->Arena = Game->Arena;
    Scene->SceneState = 0;
    Scene->Game = Game;
    Scene->Initialized = false;
}

INTERNAL_FUNCTION int FindSceneByName(char* Name)
{
    int Result = 0;
    
    for(int SceneIndex = 0;
        SceneIndex < ArrLen(MetaScene_Names);
        SceneIndex++)
    {
        if(StringsAreEqual(Name, (char*)MetaScene_Names[SceneIndex]))
        {
            Result = SceneIndex;
            break;
        }
    }
    
    return(Result);
}

INTERNAL_FUNCTION void InitGameModes(game* Game)
{
    int NumNames = ArrLen(MetaScene_Names);
    int NumInitFunctions = ArrLen(MetaScene_InitFunctions);
    int NumUpdateFunctions = ArrLen(MetaScene_UpdateFunctions);
    
    Assert(NumNames == NumInitFunctions &&
           NumNames == NumUpdateFunctions);
    
    Game->Scenes = PushArray(Game->Arena, scene, NumNames);
    Game->NumScenes = NumNames;
    
    for(int SceneIndex = 0;
        SceneIndex < Game->NumScenes;
        SceneIndex++)
    {
        InitScene(Game, SceneIndex);
    }
}

INTERNAL_FUNCTION void SaveGlobalVariables(game* Game)
{
    // NOTE(Dima): Remember subsystems into Game structure
    Game->Time = Global_Time;
    Game->Input = Global_Input;
    Game->Assets = Global_Assets;
    Game->UI = Global_UI;
    Game->RenderCommands = Global_RenderCommands;
    
    // NOTE(Dima): Initially skipped JobSystem here because it is allocated on platform layer
    
#if defined(INTERNAL_BUILD)
    Game->Debug = Global_Debug;
    Game->DebugTable = Global_DebugTable;
#endif
}

INTERNAL_FUNCTION void RestoreGlobalVariables(game* Game)
{
    PlatformAPI = *Game->PlatformAPI;
    RenderAPI = *Game->RenderAPI;
    
    Global_Time = Game->Time;
    Global_Input = Game->Input;
    Global_Assets = Game->Assets;
    Global_UI = Game->UI;
    Global_RenderCommands = Game->RenderCommands;
    Global_Jobs = Game->JobSystem;
    
#if defined(INTERNAL_FUNCTION)
    Global_Debug = Game->Debug;
    Global_DebugTable = Game->DebugTable;
#endif
}

INTERNAL_FUNCTION void ShowUI(game* Game)
{
    // TODO(Dima): Start new frame here
    if(BeginLayout("MainLayout"))
    {
        ShowText("Test format string %d, %f", 100, 123.0f);
        ShowText("Hello Twitch and YouTube!");
        ShowText("Time: %.2f", Global_Time->Time);
        ShowText("FPS: %.0f", 1.0f / Global_Time->DeltaTime);
        ShowText("FrameTime ms: %.2f", Global_Time->DeltaTime * 1000.0f);
        
        EndLayout();
    }
}

extern "C" __declspec(dllexport) GAME_INIT(GameInit)
{
    Game->Arena = Arena;
    
    // NOTE(Dima): Init subsystems
    Game->PlatformAPI = Platform;
    PlatformAPI = *Platform;
    
    Game->RenderAPI = Render;
    RenderAPI = *Render;
    
    Global_Time = PushStruct(Arena, time_system);
    Global_Jobs = Game->JobSystem;
    InitInput(Arena);
    InitAssetSystem(Arena);
    InitUI(Arena);
    InitRender(Arena, WindowDimensions);
    DEBUGInit(Arena);
    
    SaveGlobalVariables(Game);
    
    InitGameModes(Game);
    
    InitGameObjectPool(Game, Arena);
    
    //Game->CurrentSceneIndex = FindSceneByName("RubiksCube");
    Game->CurrentSceneIndex = FindSceneByName("TestGame");
    //Game->CurrentSceneIndex = FindSceneByName("Minecraft");
    //Game->CurrentSceneIndex = FindSceneByName("GraphShow");
    
    Game->NextSceneIndex = Game->CurrentSceneIndex;
}

extern "C" __declspec(dllexport) GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    // NOTE(Dima): Restoring Global variables
    if(Game->ShouldReloadGameCode)
    {
        Game->GameCodeWasJustReloaded = true;
        Game->ShouldReloadGameCode = false;
        
        RestoreGlobalVariables(Game);
        
        // NOTE(Dima): Restoring scenes function pointers
        for(int SceneIndex = 0;
            SceneIndex < Game->NumScenes;
            SceneIndex++)
        {
            InitSceneMethods(Game, SceneIndex);
        }
    }
    
    // NOTE(Dima): This should happen after we processed reloading game code!!!
    FUNCTION_TIMING();
    
    render_commands* Commands = Global_RenderCommands;
    
    // NOTE(Dima): Processing Input
    PlatformAPI.ProcessInput();
    
    UIBeginFrame(Game->WindowDimensions);
    
    // NOTE(Dima): Updating game
    BeginRender(Game->WindowDimensions,
                Global_Time->Time);
    
    scene* Scene = Game->Scenes + Game->CurrentSceneIndex;
    if(Scene->Update)
    {
        if(!Scene->Initialized)
        {
            if(Scene->Init)
            {
                Scene->Init(Scene);
            }
            
            Scene->Initialized = true;
        }
        
        Scene->Update(Scene);
    }
    
    if(GetKey(Key_LeftShift) && GetKey(Key_LeftControl))
    {
        // NOTE(Dima): Shadows enable/disable
        if(GetKeyDown(Key_O))
        {
            Commands->Lighting.DirLit.CalculateShadows = !Commands->Lighting.DirLit.CalculateShadows;
        }
        
        // NOTE(Dima): SSAO enable/disable
        if(GetKeyDown(Key_I))
        {
            post_processing* PP = &Commands->PostProcessing;
            
            b32 NewSSAOEnabled = !PostProcEffect_IsEnabled(PP, "MainSSAO");
            PostProcEffect_SetEnabled(PP, "MainSSAO", NewSSAOEnabled);
            PostProcEffect_SetEnabled(PP, "InWaterSSAO", NewSSAOEnabled);
        }
        
        if(GetKeyDown(Key_N))
        {
            ChangeScene(Game, (Game->CurrentSceneIndex + 1) % Game->NumScenes);
        }
        else if(GetKeyDown(Key_P))
        {
            int NewSceneIndex = Game->CurrentSceneIndex - 1;
            if(NewSceneIndex < 0)
            {
                NewSceneIndex = Game->NumScenes - 1;
            }
            ChangeScene(Game, NewSceneIndex);
        }
        
        if (GetKeyDown(Key_U))
        {
            Commands->TypeColorOutput = (Commands->TypeColorOutput + 1) % ColorOutput_Count;
        }
        
        if(GetMod(KeyMod_LeftAlt))
        {
            if(GetKeyDown(Key_C))
            {
                PlatformAPI.SetCapturingMouse(!Global_Input->CapturingMouse);
                
                Global_Input->CapturingMouse = !Global_Input->CapturingMouse;
            }
            if(GetKeyDown(Key_D))
            {
                DEBUGToggleShowMenus();
            }
        }
    }
    
    if(GetKeyDown(Key_Backquote))
    {
        DEBUGToggleShowMenus();
    }
    
    if(Game->NextSceneIndex != Game->CurrentSceneIndex)
    {
        Game->CurrentSceneIndex = Game->NextSceneIndex;
    }
    
    // NOTE(Dima): Debug update
    DEBUGSkipToNextBarrier(Game->GameCodeWasJustReloaded);
    DEBUGUpdate();
    
    // NOTE(Dima): Render everything
    //Commands->Rects2D_Window->TextureAtlas = 0;
    //Commands->Rects2D_Unit->TextureAtlas = 0;
    
    asset_id VoxelImageID = GetAssetID("Image_VoxelAtlasCool");
    Commands->VoxelAtlas = G_GetAssetDataByID(VoxelImageID, image);
    
    RenderAll();
    
    {
        BLOCK_TIMING("Presenting (Swapping Buffers)");
        
        // NOTE(Dima): Swapping buffers
        RenderAPI.Present(Global_RenderCommands);
    }
    
    Game->GameCodeWasJustReloaded = false;
}