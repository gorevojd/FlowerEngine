#include "prj_minecraft.cpp"

struct minecraft_state
{
    minecraft Minecraft;
    
    game_camera Camera;
};

SCENE_INIT(Minecraft)
{
    minecraft_state* State = GetSceneState(minecraft_state);
    
    InitCamera(&State->Camera, Camera_FlyAround, 0.5f, 1500.0f);
    
    CreateMinecraft(Scene->Game->Arena, &State->Minecraft);
}

SCENE_UPDATE(Minecraft)
{
    FUNCTION_TIMING();
    minecraft_state* State = GetSceneState(minecraft_state);
    
    PushClear(V3(0.8f, 0.85f, 0.95f));
    
    // NOTE(Dima): Speed multiplyer
    float SpeedMultiplier = 5.0f;
    if(GetKey(Key_LeftShift))
    {
        SpeedMultiplier *= 5.0f;
    }
    if(GetKey(Key_Space))
    {
        SpeedMultiplier *= 20.0f;
    }
    
    //PushImage(&Global_Assets->VoxelAtlas, V2(0.0f, 0.0f), 400);
    
    // NOTE(Dima): Updating camera
    UpdateCamera(&State->Camera, SpeedMultiplier);
    
    UpdateMinecraft(&State->Minecraft, State->Camera.P);
    
    SetMatrices(&State->Camera);
}