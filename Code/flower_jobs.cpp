INTERNAL_FUNCTION void KickJob(job_callback* Callback, void* Data, u32 Priority)
{
    job_queue* Queue = Global_Jobs->Queues + Priority;
    
    Queue->Lock->lock();
    
    // NOTE(Dima): Checking if we can to insert
    u32 NewAddIndex = (Queue->AddIndex + 1) % Queue->JobsCount;
    Assert(NewAddIndex != Queue->DoIndex);
    
    // NOTE(Dima): Init Job
    job* Job = &Queue->Jobs[Queue->AddIndex];
    Job->Callback = Callback;
    Job->Data = Data;
    
    // NOTE(Dima): Setting new add index
    Queue->AddIndex = NewAddIndex;
    
    Queue->Started++;
    Queue->Sygnal->notify_all();
    
    Queue->Lock->unlock();
}

INTERNAL_FUNCTION void WaitForCompletion(u32 Priority)
{
    job_queue* Queue = Global_Jobs->Queues + Priority;
    
    while(Queue->Started != Queue->Finished)
    {
        ShouldSleepAfterPerformJob(Queue);
    }
    
    Queue->Started = 0;
    Queue->Finished = 0;
}