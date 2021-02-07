#ifndef FLOWER_JOBS_H
#define FLOWER_JOBS_H

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

#define MAX_JOBS_COUNT 4096
struct jobs_layer
{
    std::mutex 
        
        job* Jobs;
    std::atomic_uint AddIndex;
    std::atomic_uint DoIndex;
};

struct job_system
{
    jobs_layer Layers[JobPriority_Count];
};

#endif //FLOWER_JOBS_H
