#include "flower_ui_graphs.cpp"

struct ui_graphs_state
{
    std::vector<v2> BlueNoise;
};

SCENE_INIT(GraphShow)
{
    ui_graphs_state* State = GetSceneState(ui_graphs_state);
    
    State->BlueNoise = GenerateBlueNoise(V2(1.0f), 0.1f, 126);
}

enum pixels_graph_show_type
{
    PixelsShow_Init,
    PixelsShow_InvertVert,
    PixelsShow_InvertHorz,
    
    PixelsShow_CacheInvertHorz,
    PixelsShow_CacheInvertVert,
    
    PixelsShow_Count,
};

inline b32 FindPixelInCache(int* CacheLinesStarts, 
                            int CacheLinesCurCount,
                            int x, int y,
                            int GraphWidth)
{
    b32 Found = false;
    
    int PixelIndex = y * GraphWidth + x;
    
    for(int i = 0; i < CacheLinesCurCount; i++)
    {
        int Diff = PixelIndex - CacheLinesStarts[i];
        
        if(Diff >= 0 && Diff < 16)
        {
            Found = true;
            break;
        }
    }
    
    return(Found);
}

void ShowPixelsGraph()
{
    image* Image = &Global_Assets->Scenery;
    
    f32 ImageScale = 0.7f;
    v2 ImageDim = V2(Image->Width,
                     Image->Height) * ImageScale;
    f32 CellDim = 15.0f;
    static ui_cell_grid_graph Graph = CreateCellGridGraph(ImageDim, CellDim);
    static f32 NextEventTime = 9999999.0f;
    static int ShowPixelsCount = 0;
    f32 EventDelay = 0.05f;
    static u32 ShowType = 0;
    static b32 ThisInCache = false;
    static b32 FastMode = false;
    static f32 Speed = 2.0f;
    static b32 PrevCacheMiss = false;
    
    static int CacheLinesStarts[10] = 
    {
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
    };
    static int CacheLinesCount = 0;
    static int CurCacheLine = 0;
    
    if(GetKeyDown(Key_Space))
    {
        NextEventTime = Global_Time->Time + EventDelay;
    }
    
    if(GetKeyDown(Key_T))
    {
        ShowType = (ShowType + 1) % PixelsShow_Count;
    }
    
    if(GetKeyDown(Key_F))
    {
        FastMode = !FastMode;
        
        if(FastMode)
        {
            Speed = 15.0f;
        }
        else
        {
            Speed = 2.0f;
        }
    }
    
    if(GetKeyDown(Key_R))
    {
        NextEventTime = 9999999.0f;
        ShowPixelsCount = 0;
        
        // NOTE(Dima): Reset caches
        for(int i = 0; i < ArrayCount(CacheLinesStarts); i++)
        {
            CacheLinesStarts[i] = 0;
        }
        CacheLinesCount = 0;
    }
    
    // NOTE(Dima): Processing next event
    if(Global_Time->Time > NextEventTime)
    {
        switch(ShowType)
        {
            
            case PixelsShow_Init:
            case PixelsShow_InvertVert:
            case PixelsShow_InvertHorz:
            {
                ShowPixelsCount = MinFloat(ShowPixelsCount + 1, Graph.VertCount * Graph.HorzCount);
            }break;
            
            case PixelsShow_CacheInvertHorz:
            case PixelsShow_CacheInvertVert:
            {
                
                f32 AdditionalDelay = 0.0f;
                
                if(PrevCacheMiss)
                {
                    PrevCacheMiss = false;
                    
                    int CurLine = CurCacheLine;
                    int CacheLineStart = ShowPixelsCount;
                    if(ShowType == PixelsShow_CacheInvertVert)
                    {
                        int x = ShowPixelsCount / Graph.VertCount;
                        int y = ShowPixelsCount % Graph.VertCount;
                        
                        CacheLineStart = y * Graph.HorzCount + x;
                    }
                    CacheLinesStarts[CurLine] = CacheLineStart;
                    
                    CurCacheLine = (CurCacheLine + 1) % ArrayCount(CacheLinesStarts);
                    CacheLinesCount = std::min(CacheLinesCount + 1, (int)ArrayCount(CacheLinesStarts));
                    
                    AdditionalDelay = 0.4f;
                }
                
                
                int ThisX = ShowPixelsCount % Graph.HorzCount;
                int ThisY = ShowPixelsCount / Graph.HorzCount;
                if(ShowType == PixelsShow_CacheInvertVert)
                {
                    ThisX = ShowPixelsCount / Graph.VertCount;
                    ThisY = ShowPixelsCount % Graph.VertCount;
                }
                
                b32 InCache = FindPixelInCache(CacheLinesStarts, 
                                               CacheLinesCount,
                                               ThisX, ThisY,
                                               Graph.HorzCount);
                
                if(InCache)
                {
                    ThisInCache = true;
                    ShowPixelsCount = MinFloat(ShowPixelsCount + 1, Graph.VertCount * Graph.HorzCount);
                }
                else
                {
                    ThisInCache = false;
                    
                    if(!PrevCacheMiss)
                    {
                        AdditionalDelay = 1.0f;
                    }
                    
                    PrevCacheMiss = true;
                }
                
                NextEventTime = Global_Time->Time + (EventDelay + AdditionalDelay) / Speed;
            }break;
        }
    }
    
    // NOTE(Dima): Processing update
    switch(ShowType)
    {
        case PixelsShow_Init:
        {
            for(int y = 0; y < Graph.VertCount; y++)
            {
                for(int x = 0; x < Graph.HorzCount; x++)
                {
                    int PixelIndex = Graph.HorzCount * y + x;
                    
                    v4 PixelColor = ColorBlack();
                    if(PixelIndex < ShowPixelsCount)
                    {
                        int PixelX = x * Graph.CellDim / ImageScale;
                        int PixelY = y * Graph.CellDim / ImageScale;
                        
                        PixelColor = GetPixelColor(Image, PixelX, PixelY);
                    }
                    
                    rc2 Rect = GetCellRect(&Graph, x, y);
                    
                    PushRect(Rect, PixelColor);
                }
            }
        }break;
        
        case PixelsShow_CacheInvertHorz:
        case PixelsShow_InvertHorz:
        {
            for(int y = 0; y < Graph.VertCount; y++)
            {
                for(int x = 0; x < Graph.HorzCount; x++)
                {
                    int PixelIndex = Graph.HorzCount * y + x;
                    int PixelX = x * Graph.CellDim / ImageScale;
                    int PixelY = y * Graph.CellDim / ImageScale;
                    
                    v4 PixelColor = GetPixelColor(Image, PixelX, PixelY);
                    if(PixelIndex < ShowPixelsCount)
                    {
                        PixelColor = InvertColor(GetPixelColor(Image, PixelX, PixelY));
                    }
                    
                    rc2 Rect = GetCellRect(&Graph, x, y);
                    
                    PushRect(Rect, PixelColor);
                }
            }
        }break;
        
        case PixelsShow_CacheInvertVert:
        case PixelsShow_InvertVert:
        {
            for(int x = 0; x < Graph.HorzCount; x++)
            {
                for(int y = 0; y < Graph.VertCount; y++)
                {
                    int PixelIndex = Graph.VertCount * x + y;
                    int PixelX = x * Graph.CellDim / ImageScale;
                    int PixelY = y * Graph.CellDim / ImageScale;
                    
                    v4 PixelColor = GetPixelColor(Image, PixelX, PixelY);
                    if(PixelIndex < ShowPixelsCount)
                    {
                        PixelColor = InvertColor(GetPixelColor(Image, PixelX, PixelY));
                    }
                    
                    rc2 Rect = GetCellRect(&Graph, x, y);
                    
                    PushRect(Rect, PixelColor);
                }
            }
        }break;
    }
    
    if(ShowType == PixelsShow_CacheInvertHorz ||
       ShowType == PixelsShow_CacheInvertVert)
    {
        // NOTE(Dima): Showing cache lines
        for(int CacheLineIndex = 0;
            CacheLineIndex < CacheLinesCount;
            CacheLineIndex++)
        {
            for(int RectIndex = 0;
                RectIndex < 16;
                RectIndex++)
            {
                int PixelIndex = CacheLinesStarts[CacheLineIndex] + RectIndex;
                
                int x = PixelIndex % Graph.HorzCount;
                int y = PixelIndex / Graph.HorzCount;
                
                v4 Color = ColorYellow();
                Color.a = 0.5f;
                
                rc2 Rect = GetCellRect(&Graph, x, y);
                PushRect(Rect, Color);
            }
        }
        
        int ThisX = ShowPixelsCount % Graph.HorzCount;
        int ThisY = ShowPixelsCount / Graph.HorzCount;
        if(ShowType == PixelsShow_CacheInvertVert)
        {
            ThisX = ShowPixelsCount / Graph.VertCount;
            ThisY = ShowPixelsCount % Graph.VertCount;
        }
        
        // NOTE(Dima): Color current based on if it missed or hit
        v4 ThisColor = ColorRed();
        if(FindPixelInCache(CacheLinesStarts, 
                            CacheLinesCount,
                            ThisX, ThisY, 
                            Graph.HorzCount))
        {
            ThisColor = ColorGreen();
        }
        
        rc2 ThisRect = GetCellRect(&Graph, ThisX, ThisY);
        
        PushRect(ThisRect, ThisColor);
    }
    
    ShowCellGrid(&Graph);
}

void PrintTechniques(int Count,
                     v2* TargetPositions,
                     v2* InitPositions,
                     int* Indices,
                     f32 StartTime,
                     f32 TimeToMove,
                     char** TechniquesNames,
                     char* Logo)
{
    v2 FirstPoint = TargetPositions[0];
    
    v2 LogoAt = V2(FirstPoint.x, FirstPoint.y - GetLineAdvance());
    f32 AlphaT = SmoothstepFloat(Clamp01Float((Global_Time->Time - StartTime) / TimeToMove));
    v4 LogoColor = ColorYellow();
    LogoColor.a = AlphaT;
    
    PrintTextAligned(Logo, LogoAt,
                     TextAlign_Center,
                     TextAlign_Center,
                     LogoColor);
    
    for(int i = 0; i < Count; i++)
    {
        v2 TargetP = TargetPositions[i];
        v2 InitP = InitPositions[Indices[i]];
        
        f32 Diff = Global_Time->Time - StartTime - ((f32)i * 0.1f);
        v2 P = InitP;
        if(Diff > 0.0f)
        {
            float t = Clamp01Float(Diff / TimeToMove);
            
            t = SmoothstepFloat(t);
            
            P = Lerp(InitP, TargetP, t);
        }
        
        PrintTextAligned(TechniquesNames[i], 
                         P,
                         TextAlign_Center,
                         TextAlign_Center);
    }
}

void ShowOptimizeTechniquesGraph()
{
    UIPushFont(&Global_Assets->BerlinSans);
    
    PushClear(ColorGray(0.1f).rgb);
    
    char* CPUTechniques[] = {
        "Data locality",
        "SOA vs AOS",
        "Multithreading",
        "SIMD",
        "Frustum culling",
        "Compilation time speedup",
    };
    
    char* GPUTechniques[] = {
        "Batching",
        "Instancing",
        "Texture Atlases & Arrays",
        "Texture Arrays",
        "Occlusion culling",
    };
    
    int CPUCount = ArrayCount(CPUTechniques);
    int GPUCount = ArrayCount(GPUTechniques);
    int TotalCount = ArrayCount(CPUTechniques) + ArrayCount(GPUTechniques);
    
    static b32 IsInit;
    static v2* Positions;
    static int* IndicesCPU;
    static int* IndicesGPU;
    static v2* TargetCPU;
    static v2* TargetGPU;
    
    static f32 StartTimeCPU = 99999.0f;
    static f32 StartTimeGPU = 99999.0f;
    static b32 CPUStarted = false;
    
    f32 TimeToMove = 2.0f;
    
    if(GetKeyDown(Key_Space))
    {
        if(CPUStarted)
        {
            StartTimeGPU = Global_Time->Time;
        }
        else
        {
            StartTimeCPU = Global_Time->Time;
            CPUStarted = true;
        }
    }
    
    if(GetKeyDown(Key_R))
    {
        StartTimeCPU = 9999999.0f;
        StartTimeGPU = 9999999.0f;
        CPUStarted = false;
    }
    
    if(!IsInit)
    {
        IsInit = true;
        
        memory_arena Arena = {};
        Positions = PushArray(&Arena, v2, TotalCount);
        IndicesCPU = PushArray(&Arena, int, CPUCount);
        IndicesGPU = PushArray(&Arena, int, GPUCount);
        TargetCPU = PushArray(&Arena, v2, CPUCount);
        TargetGPU = PushArray(&Arena, v2, GPUCount);
        
        int CurrentIndex = 0;
        
        // NOTE(Dima): Init center positions
        f32 StartPrintY = UVToScreenPoint(0.0f, 0.2f).y;
        f32 AtY = StartPrintY;
        
        f32 CenterX = UVToScreenPoint(0.5f, 0.0f).x;
        int CurCPU = 0;
        int CurGPU = 0; 
        while(CurCPU + CurGPU < TotalCount)
        {
            if(CurCPU < CPUCount)
            {
                Positions[CurrentIndex] = V2(CenterX, AtY);
                IndicesCPU[CurCPU] = CurrentIndex;
                
                AtY += GetLineAdvance();
                CurrentIndex++;
                CurCPU++;
            }
            
            if(CurGPU < GPUCount)
            {
                Positions[CurrentIndex] = V2(CenterX, AtY);
                IndicesGPU[CurGPU] = CurrentIndex;
                
                AtY += GetLineAdvance();
                CurrentIndex++;
                CurGPU++;
            }
        }
        
        f32 Center33 = UVToScreenPoint(0.25f, 0.0f).x;
        f32 Center66 = UVToScreenPoint(0.75, 0.0f).x;
        
        // NOTE(Dima): Calculate target CPU positions
        f32 StartPrintCPU = UVToScreenPoint(0.0f, 0.25f).y;
        for(int i = 0; i < CPUCount; i++)
        {
            f32 TargetY = StartPrintCPU + i * GetLineAdvance();
            
            TargetCPU[i] = V2(Center33, TargetY);
        }
        
        // NOTE(Dima): Calculate target GPU positions
        f32 StartPrintGPU = UVToScreenPoint(0.0f, 0.25f).y;
        for(int i = 0; i < GPUCount; i++)
        {
            f32 TargetY = StartPrintGPU + i * GetLineAdvance();
            
            TargetGPU[i] = V2(Center66, TargetY);
        }
    }
    
    // NOTE(Dima): Rendering CPU techniques
    PrintTechniques(CPUCount,
                    TargetCPU,
                    Positions,
                    IndicesCPU,
                    StartTimeCPU,
                    TimeToMove,
                    CPUTechniques,
                    "CPU");
    
    // NOTE(Dima): Rendering GPU techniques
    PrintTechniques(GPUCount,
                    TargetGPU,
                    Positions,
                    IndicesGPU,
                    StartTimeGPU,
                    TimeToMove,
                    GPUTechniques,
                    "GPU");
    
    UIPopFont();
}

void GraphThreadPool()
{
    static b32 IsInit = false;
    
    struct graph_pool_worker
    {
        rc2 Rect;
        f32 StartLifeTime;
        v2 CenterP;
        
        b32 ExecutingJob;
        f32 ExecuteJobEnd;
        int ExecubeJobIndex;
    };
    
    struct graph_pool_job
    {
        v2 CurP;
        v2 TargetP;
        
        f32 Dim;
        f32 TimeToPerform;
        
        f32 StartLifeTime;
        f32 EndLifeTime;
        b32 JustCreated;
    };
    
    f32 JobsFinishP = 0.7f;
    f32 ThreadsColumnX = 0.9f;
    v2 DimThread = UVToScreenPoint(0.15f, 0.08f);
    int ThreadCount = 8;
    f32 AppearTime = 1.0f;
    f32 SpacingBetweenJobs = 10.0f;
    int MaxJobsCount = 1000;
    f32 JobMinDim = 20;
    f32 JobMaxDim = 80;
    f32 JobMinTime = 0.5f;
    f32 JobMaxTime = 1.5f;
    
    
    static f32 LastPeekJob = -999999.0f;
    static f32 NextJobGenerate = 999999.0f;
    static graph_pool_worker* Workers = 0;
    static graph_pool_job* Jobs = 0;
    static int* FreeWorkers = 0;
    static memory_arena Arena = {};
    static int SpacesPressed = 0;
    static int JobsCount = 0;
    static int FirstJobIndex;
    static random_generation Random;
    static b32 CanPeekJobs = false;
    static int ExecuteJobsCount = 0;
    
    if(!IsInit)
    {
        IsInit = true;
        
        Random = SeedRandom(2134);
        
        Workers = PushArray(&Arena, graph_pool_worker, ThreadCount);
        Jobs = PushArray(&Arena, graph_pool_job, MaxJobsCount);
        FreeWorkers = PushArray(&Arena, int, ThreadCount);
        JobsCount = 0;
        
        // NOTE(Dima): Init thread rects 
        f32 StartY = 0.1f;
        f32 EndY = 0.9f;
        f32 At = StartY;
        for(int i = 0; i < ThreadCount; i++)
        {
            Workers[i].CenterP = UVToScreenPoint(ThreadsColumnX, At);
            Workers[i].Rect = RectCenterDim(Workers[i].CenterP, DimThread);
            
            Workers[i].StartLifeTime = 999999.0f;
            Workers[i].ExecutingJob = false;
            
            At += (EndY - StartY) / ((f32)ThreadCount - 1.0f);
        }
        
        // NOTE(Dima): Init jobs
        for(int JobIndex = 0;
            JobIndex < MaxJobsCount;
            JobIndex++)
        {
            graph_pool_job* Job = &Jobs[JobIndex];
            
            Job->StartLifeTime = 999999.0f;
            Job->EndLifeTime = 999999.0f;
        }
    }
    
    b32 NeedResetJobsPositions = false;
    
    // NOTE(Dima): Next step
    if(GetKeyDown(Key_Space))
    {
        if(SpacesPressed == 0)
        {
            for(int WorkerIndex = 0; WorkerIndex < ThreadCount; WorkerIndex++)
            {
                graph_pool_worker* Worker = &Workers[WorkerIndex];
                
                Worker->StartLifeTime = Global_Time->Time + (f32)WorkerIndex * 0.2f;
            }
        }
        
        if(SpacesPressed == 1)
        {
            FirstJobIndex = 0;
            JobsCount = 12;
            
            f32 JobAt = UVToScreenPoint(JobsFinishP, 0.5f).x;
            
            for(int Index = 0;
                Index < JobsCount;
                Index++)
            {
                int JobIndex = (FirstJobIndex + Index) % MaxJobsCount;
                graph_pool_job* Job = &Jobs[JobIndex];
                
                Job->StartLifeTime = Global_Time->Time + (f32)JobIndex * 0.2f;
                
                Job->TimeToPerform = Lerp(JobMinTime, JobMaxTime, RandomUnilateral(&Random));
                Job->Dim = Lerp(JobMinDim, JobMaxDim, RandomUnilateral(&Random));
                Job->JustCreated = true;
                
                JobAt -= (Job->Dim + SpacingBetweenJobs);
            }
            
            NeedResetJobsPositions = true;
        }
        
        if(SpacesPressed == 2)
        {
            CanPeekJobs = true;
            NextJobGenerate = Global_Time->Time + 1.0f;
        }
        
        SpacesPressed++;
    }
    
    PushClear(V3(0.9f));
    
    
    // NOTE(Dima): Finding free workers
    int FreeWorkersCount = 0;
    for(int WorkerIndex = 0; WorkerIndex < ThreadCount; WorkerIndex += 1)
    {
        graph_pool_worker* Worker = &Workers[WorkerIndex];
        
        if(Worker->ExecutingJob == false)
        {
            FreeWorkers[FreeWorkersCount++] = WorkerIndex;
        }
    }
    
    // NOTE(Dima): Generating jobs
    f32 GenerateJobDelay = 1.0f;
    
    if(CanPeekJobs && 
       (Global_Time->Time > NextJobGenerate))
    {
        int GenJobsCount = RandomBetweenU32(&Random, 4, 9);
        
        for(int i = 0; i < GenJobsCount; i++)
        {
            int JobIndex = (FirstJobIndex + JobsCount + i) % MaxJobsCount;
            graph_pool_job* Job = &Jobs[JobIndex];
            
            Job->StartLifeTime = Global_Time->Time + (f32)i * 0.2f;
            
            Job->TimeToPerform = Lerp(JobMinTime, JobMaxTime, RandomUnilateral(&Random));
            Job->Dim = Lerp(JobMinDim, JobMaxDim, RandomUnilateral(&Random));
            Job->JustCreated = true;
        }
        
        JobsCount += GenJobsCount;
        
        NextJobGenerate = Global_Time->Time + GenerateJobDelay;
        
        NeedResetJobsPositions = true;
    }
    
    // NOTE(Dima): Process peeking job
    f32 PeekJobDelay = 0.2f;
    if(CanPeekJobs && (FreeWorkersCount > 0) && (JobsCount > 0) &&
       ((Global_Time->Time - LastPeekJob) > PeekJobDelay))
    {
        int RandomWorkerIndex = RandomIndex(&Random, FreeWorkersCount);
        
        graph_pool_worker* Worker = &Workers[FreeWorkers[RandomWorkerIndex]];
        
        Assert(!Worker->ExecutingJob);
        
        int JobIndex = FirstJobIndex;
        graph_pool_job* Job = &Jobs[JobIndex];
        FirstJobIndex = (FirstJobIndex + 1) % MaxJobsCount;
        JobsCount--;
        
        Job->TargetP = Worker->CenterP;
        Worker->ExecutingJob = true;
        Worker->ExecuteJobEnd = Global_Time->Time + Job->TimeToPerform;
        Worker->ExecubeJobIndex = JobIndex;
        
        ExecuteJobsCount++;
        LastPeekJob = Global_Time->Time;
        
        NeedResetJobsPositions = true;
    }
    
    if(NeedResetJobsPositions)
    {
        v2 JobAt = UVToScreenPoint(JobsFinishP, 0.5f);
        
        for(int Index = 0;
            Index < JobsCount;
            Index++)
        {
            int JobIndex = (FirstJobIndex + Index) % MaxJobsCount;
            graph_pool_job* Job = &Jobs[JobIndex];
            
            v2 P = V2(JobAt.x - Job->Dim * 0.5f, JobAt.y);
            
            if(Job->JustCreated)
            {
                Job->JustCreated = false;
                Job->CurP = P;
            }
            Job->TargetP = P;
            
            JobAt.x -= (Job->Dim + SpacingBetweenJobs); 
        }
    }
    
    // NOTE(Dima): Printing workerThreads
    for (int WorkerIndex = 0; WorkerIndex < ThreadCount; WorkerIndex += 1)
    {
        graph_pool_worker* Worker = &Workers[WorkerIndex];
        
        // NOTE(Dima): Process executing job
        if(Worker->ExecutingJob && (Global_Time->Time > Worker->ExecuteJobEnd))
        {
            Worker->ExecutingJob = false;
            Jobs[Worker->ExecubeJobIndex].EndLifeTime = Global_Time->Time;
        }
        
        // NOTE(Dima): Printing rect
        f32 DiffTime = Global_Time->Time - Worker->StartLifeTime;
        f32 Alpha = Clamp01Float(DiffTime / AppearTime);
        
        Alpha = SmoothstepFloat(Alpha);
        
        char WorkerName[64];
        stbsp_sprintf(WorkerName, "Thread %d", WorkerIndex + 1);
        
        v4 Color = V4(0.2f, 0.5f, 1.0f, Alpha);
        if(Worker->ExecutingJob)
        {
            Color = V4(ColorRed().rgb, Alpha);
        }
        
        PushRect(Worker->Rect, Color);
        
        PrintTextAligned(WorkerName, Worker->Rect,
                         TextAlign_Center,
                         TextAlign_Center,
                         V4(ColorWhite().rgb, Alpha));
        
        PushRectOutline(Worker->Rect, 2.0f, V4(ColorBlack().rgb, Alpha));
    }
    
    // NOTE(Dima): Printing jobs
    int CurExecuteJobs = ExecuteJobsCount;
    int CurJobsCount = JobsCount + ExecuteJobsCount;
    for(int Index = 0;
        Index < CurJobsCount;
        Index++)
    {
        int StartJobIndex = FirstJobIndex - CurExecuteJobs;
        if(StartJobIndex < 0)
        {
            StartJobIndex += MaxJobsCount;
        }
        int JobIndex = (StartJobIndex + Index) % MaxJobsCount;
        
        graph_pool_job* Job = &Jobs[JobIndex];
        
        f32 DiffTime = Global_Time->Time - Job->StartLifeTime;
        f32 Alpha = Clamp01Float(DiffTime / AppearTime);
        
        b32 IsEnd = false;
        f32 EndLifeDiff = Global_Time->Time - Job->EndLifeTime;
        if(EndLifeDiff > 0.0f)
        {
            DiffTime = Global_Time->Time - Job->EndLifeTime;
            f32 DisappearTime = AppearTime * 0.5f;
            Alpha = Clamp01Float(DiffTime / DisappearTime);
            
            IsEnd = true;
            
            if(EndLifeDiff > DisappearTime)
            {
                ExecuteJobsCount--;
            }
        }
        
        Alpha = SmoothstepFloat(Alpha);
        
        if(IsEnd)
        {
            Alpha = 1.0f - Alpha;
        }
        
        Job->CurP = Lerp(Job->CurP, Job->TargetP, Global_Time->DeltaTime * 6.0f);
        f32 Dim = Lerp(0.0f, Job->Dim, Alpha);
        rc2 JobRect = RectCenterDim(Job->CurP, V2(Dim));
        
        PushRect(JobRect, V4(ColorGreen().rgb, Alpha));
        PushRectOutline(JobRect, 2.0f, V4(ColorBlack().rgb, Alpha));
    }
}


void GraphThreads()
{
    PushImage(&Global_Assets->MultiCore, V2(0.0f, 0.0f), 900);
    
    static f32 Lanes[] = {
        0.3f, 0.59f, 
    };
    
    static b32 IsInit = false;
    static f32** PositionsArrays = 0;
    static f32 LastSwitchTime = -9999.0f;
    static int CountToExecute = 6;
    static int ExecuteLaneIndex = 0;
    static memory_arena Arena = {};
    static random_generation Random;
    static b32 WasWaiting = false;
    static b32 LastSwitchIsEndWait = false;
    
#if 0    
    int Count = ArrayCount(Lanes);
#else
    int Count = 2;
#endif
    
    int OnLaneInstructionsCount = 30;
    f32 FinishLine = 0.8f;
    f32 ConnectPointUVx = 0.7f;
    f32 ConnectPointUVy = Lanes[0] + (Lanes[1] - Lanes[0]) * 0.5f;
    f32 FinishLineX = UVToScreenPoint(FinishLine, 0.0f).x;
    
    f32 ConnectPointX = UVToScreenPoint(ConnectPointUVx, 0.0f).x;
    v2 EndPoint = UVToScreenPoint(FinishLine, ConnectPointUVy);
    
    f32 InDim = 20.0f;
    f32 InHalfDim = InDim * 0.5f;
    f32 StepBetweenInstructions = FinishLineX / (f32)OnLaneInstructionsCount;
    
    if(!IsInit)
    {
        IsInit = true;
        
        Random = SeedRandom(1234);
        
        PositionsArrays = PushArray(&Arena, f32*, Count);
        for(int Lane = 0;
            Lane < Count;
            Lane++)
        {
            PositionsArrays[Lane] = PushArray(&Arena, f32, OnLaneInstructionsCount);
            
            f32* Positions = PositionsArrays[Lane];
            
            f32 At = -InDim * 0.5f;
            for(int i = 0; i < OnLaneInstructionsCount; i++)
            {
                Positions[i] = At;
                
                At += StepBetweenInstructions;
            }
        }
    }
    
    f32 WaitDelay = 1.5f;
    b32 IsWaiting = (Global_Time->Time - LastSwitchTime) < WaitDelay;
    
    if(LastSwitchIsEndWait)
    {
        IsWaiting = false;
    }
    
    if(!IsWaiting)
    {
        if(WasWaiting)
        {
            LastSwitchTime = Global_Time->Time;
            LastSwitchIsEndWait = true;
        }
    }
    WasWaiting = IsWaiting;
    
    for(int LaneIndex = 0;
        LaneIndex < Count;
        LaneIndex++)
    {
        f32* Positions = PositionsArrays[LaneIndex];
        
        f32 LaneY = UVToScreenPoint(0.0f, Lanes[LaneIndex]).y;
        
        for(int InstructionIndex = 0;
            InstructionIndex < OnLaneInstructionsCount;
            InstructionIndex++)
        {
            
            if(Positions[InstructionIndex] + InHalfDim > FinishLineX)
            {
                Positions[InstructionIndex] = -InHalfDim;
                
                CountToExecute--;
            }
            
            if(CountToExecute <= 0)
            {
                ExecuteLaneIndex = 1 - ExecuteLaneIndex;
                
                LastSwitchTime = Global_Time->Time;
                LastSwitchIsEndWait = false;
                
                CountToExecute = RandomBetweenU32(&Random, 15, 32);
            }
            
            v2 Center = V2(Positions[InstructionIndex], LaneY);
            if(Positions[InstructionIndex] > ConnectPointX)
            {
                f32 t = Clamp01Float((Positions[InstructionIndex] - ConnectPointX) / (FinishLineX - ConnectPointX));
                t = SmoothstepFloat(t);
                
                f32 NewYuv = Lerp(Lanes[LaneIndex], ConnectPointUVy, t);
                f32 NewY = UVToScreenPoint(0.0f, NewYuv).y;
                Center = V2(Positions[InstructionIndex], NewY);
            }
            
            
            v4 SrcColor;
            v4 DstColor;
            
            v4 Color0 = V4(0.1f, 0.25f, 0.5f, 1.0f);
            v4 Color1 = V4(0.2f, 0.5f, 1.0f, 1.0f);
            v4 ColorWait = ColorRed();
            if(LaneIndex == ExecuteLaneIndex)
            {
                SrcColor = Color0;
                DstColor = Color1;
                
                // NOTE(Dima): Updating position
                if(!IsWaiting)
                {
                    Positions[InstructionIndex] += Global_Time->DeltaTime * 400.0f;
                }
            }
            else
            {
                SrcColor = Color1;
                DstColor = Color0;
            }
            
            if(LastSwitchIsEndWait)
            {
                SrcColor = ColorWait;
            }
            
            if(IsWaiting)
            {
                DstColor = ColorWait;
            }
            
            f32 TimeToSwitchColor = 0.5f;
            f32 FactorColor = Clamp01Float((Global_Time->Time - LastSwitchTime) / TimeToSwitchColor);
            
            v4 Color = Lerp(SrcColor, DstColor, FactorColor);
            
            v2 Dim = V2(InDim);
            
            rc2 InstructionRect = RectCenterDim(Center, Dim);
            
            PushRect(InstructionRect, Color);
            PushRectOutline(InstructionRect, 2.0f, ColorBlack());
        }
    }
}

SCENE_UPDATE(GraphShow)
{
    ui_graphs_state* State = GetSceneState(ui_graphs_state);
    
    static f32 TimeStart = 5.0f;
    f32 FadeDuration = 7.0f;
    
    if(GetKeyDown(Key_R))
    {
        TimeStart = 9999999.0f;
    }
    
    if(GetKeyDown(Key_Space))
    {
        TimeStart = Global_Time->Time;
    }
    
#if 0    
    ShowPixelsGraph();
    ShowOptimizeTechniquesGraph();
    
    GraphThreadPool();
#endif
    
    GraphThreads();
    
#if 0
    if(GetKeyDown(Key_I))
    {
        u64 StartClocks = Platform.GetPerfCounter();
        InvertImageColors(&Global_Assets->Scenery);
        u64 EndClocks= Platform.GetPerfCounter();
        
        f64 ElapsedTime = Platform.GetElapsedTime(StartClocks, EndClocks);
        printf("%.4fms\n", ElapsedTime * 1000.0f);
    }
    
    PushImage(&Global_Assets->Scenery, V2(0.0f, 0.0f), 900);
#endif
}
