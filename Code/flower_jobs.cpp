void KickJob(job_callback* Callback, void* Data, u32 Priority)
{
    
}

// NOTE(Dima): Returns true if no jobs to perform
b32 PerformJob()
{
    b32 Result = false;
    
    if()
    {
        
    }`
        else
    {
        Result = true;
    }
    
    return(Result);
}

void JobWorkerThreadWork()
{
    for(;;)
    {
        if(PerformJob())
        {
            std::unique_lock<std::mutex> UniqueLock(ConditionVariable);
            ConditionVariable.wait(UniqueLock);
        }
    }
}

void InitJobSystem(memory_arena* Arena)
{
    Global_JobSystem = PushStruct(Arena, job_system);
    
    job_system* Jobs = Global_JobSystem;
    
    // NOTE(Dima): Init jobs arrays
    job* JobsArray = PushArray(Arena, job, MAX_JOBS_COUNT * JobPriority_Count);
    
    for(int Layer = 0;
        Layer < JobPriority_Count;
        Layer++)
    {
        jobs_layer* Layer = &Jobs->Layers[Layer];
        
        Layer->Jobs = JobsArray + Layer * MAX_JOBS_COUNT;
        Layer->AddIndex = 0;
        Layer->DoIndex = 0;
    }
}