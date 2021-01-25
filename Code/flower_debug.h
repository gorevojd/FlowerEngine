#ifndef FLOWER_DEBUG_H
#define FLOWER_DEBUG_H

#if defined(INTERNAL_BUILD)


struct debug_timing_snapshot{
    u64 StartClockFirstEntry;
    u64 StartClock;
    u64 EndClock;
    u64 ClocksElapsed;
    u32 HitCount;
};

struct debug_profiled_tree_node{
    char* UniqueName;
    u32 NameID;
    
    debug_profiled_tree_node* Parent;
    debug_profiled_tree_node* ChildSentinel;
    
    debug_profiled_tree_node* NextAlloc;
    debug_profiled_tree_node* PrevAlloc;
    
    debug_profiled_tree_node* Next;
    debug_profiled_tree_node* Prev;
    
    debug_timing_snapshot TimingSnapshot;
};

struct debug_timing_stat{
    char* UniqueName;
    u32 NameID;
    
    debug_timing_stat* Next;
    debug_timing_stat* Prev;
    
    debug_timing_stat* NextInHash;
    
    struct{
        u64 ClocksElapsed;
        u64 ClocksElapsedInChildren;
        u32 HitCount;
    } Stat;
};

#define DEBUG_PROFILED_FRAMES_COUNT 256
#define DEBUG_STATS_TABLE_SIZE 128
#define DEBUG_STATS_TO_SORT_SIZE 4096
#define DEBUG_THREADS_TABLE_SIZE 32
#define DEBUG_DEFAULT_FILTER_VALUE 0xFFFFFFFF

struct debug_common_frame{
    f32 FrameTime;
};

struct debug_thread_frame{
    debug_profiled_tree_node RootTreeNodeUse;
    debug_timing_stat StatUse;
    
    debug_timing_stat* StatTable[DEBUG_STATS_TABLE_SIZE];
    
    debug_timing_stat* ToSortStats[DEBUG_STATS_TO_SORT_SIZE];
    int ToSortStatsCount;
    
    debug_profiled_tree_node* FrameUpdateNode;
    
    debug_profiled_tree_node* CurNode;
};

struct debug_thread{
    debug_thread* NextAlloc;
    debug_thread* PrevAlloc;
    
    debug_thread* NextInHash;
    
    debug_thread_frame* Frames;
    
    char* WatchNodeUniqueName;
    
    u16 ThreadID;
};

struct debug_menus
{
    float GraphsSizeY;
    
    char* SelectedStatGUID;
    
    float* SelectedFunFloatsIncl;
    float* SelectedFunFloatsExcl;
    
    float* FPSGraph_FrameTimes;
    float* FPSGraph_FPSValues;
    
    int TopClocks_SelectedStatID;
    
    b32 Visible;
    b32 IncludingChildren;
};

struct debug_state{
    memory_arena* Arena;
    
    debug_menus Menus;
    
    // NOTE(Dima): Profiler stuff
    int CollationFrameIndex;
    int ViewFrameIndex;
    int NewestFrameIndex;
    int OldestFrameIndex;
    b32 OldestShouldBeIncremented;
    
    b32 IsRecording;
    b32 RecordingChangeRequested;
    u32 Filter;
    
    debug_profiled_tree_node TreeNodeFree;
    debug_timing_stat StatFree;
    
    debug_thread ThreadSentinel;
    debug_thread* MainThread;
    debug_thread* WatchThread;
    debug_thread* ThreadHashTable[DEBUG_THREADS_TABLE_SIZE];
    int ProfiledThreadsCount;
    
    debug_common_frame Frames[DEBUG_PROFILED_FRAMES_COUNT];
    
    char RootNodesName[32];
    char SentinelElementsName[32];
    u32 RootNodesNameHash;
    u32 SentinelElementsNameHash;
};

#endif //INTERNAL_BUILD

#endif //FLOWER_DEBUG_H
