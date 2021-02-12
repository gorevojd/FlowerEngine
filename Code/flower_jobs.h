#ifndef FLOWER_JOBS_H
#define FLOWER_JOBS_H

#include <thread>
#include <mutex>
#include <condition_variable>

#define JOB_CALLBACK(name) void name(void* Data)
typedef JOB_CALLBACK(job_callback);

enum job_priority
{
    JobPriority_Low,
    JobPriority_High,
    
    JobPriority_Count,
};

struct job
{
    job_callback* Callback;
    void* Data;
};

struct job_queue
{
#define DEFAULT_JOBS_COUNT 4096
    job* Jobs;
    int JobsCount;
    
    u32 AddIndex;
    u32 DoIndex;
    
    u64 Started;
    u64 Finished;
    
    std::mutex* Lock;
    std::mutex* SygnalLock;
    std::condition_variable* Sygnal;
    
    int CreationThreadCount;
};

struct job_system
{
    job_queue Queues[JobPriority_Count];
};

// NOTE(Dima): Returns true if no jobs to perform
inline b32 ShouldSleepAfterPerformJob(job_queue* Queue)
{
    b32 Result = false;
    
    if(Queue->AddIndex != Queue->DoIndex)
    {
        Queue->Lock->lock();
        
        // NOTE(Dima): Calling job's callback
        job* Job = &Queue->Jobs[Queue->DoIndex];
        Job->Callback(Job->Data);
        
        Queue->Finished++;
        
        Queue->DoIndex = (Queue->DoIndex + 1) % Queue->JobsCount;
        
        Queue->Lock->unlock();
    }
    else
    {
        Result = true;
    }
    
    return(Result);
}


#endif //FLOWER_JOBS_H
