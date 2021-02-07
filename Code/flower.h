#ifndef FLOWER_GAME_H
#define FLOWER_GAME_H

#include <iostream>
#include <vector>
#include <algorithm>

#include "flower_defines.h"
#include "flower_math.h"
#include "flower_random.h"
#include "flower_strings.h"
#include "flower_platform.h"
#include "flower_mem.h"

#include "flower_input.h"
#include "flower_render_primitives.h"
#include "flower_asset.h"
#include "flower_opengl.h"
#include "flower_render.h"
#include "flower_ui.h"
#include "flower_debug.h"

#include "flower_scene.h"

struct game
{
    scene* Scenes;
    int NumScenes;
    
    int CurrentSceneIndex;
    int NextSceneIndex;
    
    memory_arena* Arena;
    
    // NOTE(Dima): Subsystems
    input_system* Input;
    time* Time;
    asset_system* Assets;
    ui_state* UI;
    render_commands* RenderCommands;
    platform_api* PlatformAPI;
    
#if defined(INTERNAL_BUILD)
    debug_state* Debug;
    debug_global_table* DebugTable;
#endif
    
    window_dimensions WindowDimensions;
    
    b32 CodeDllWasJustReloaded;
};

inline void ChangeScene(game* Game, int NewSceneIndex)
{
    Game->NextSceneIndex = NewSceneIndex;
}

#define GAME_INIT(name) void name(game* Game, memory_arena* Arena, platform_api* PlatformAPI)
typedef GAME_INIT(game_init);

#define GAME_UPDATE_AND_RENDER(name) void name(game* Game)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

#endif //FLOWER_GAME_MODE_H
