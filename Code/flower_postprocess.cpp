#include "flower_postprocess_shared.cpp"

inline post_proc_effect* PostProcEffect_Add(post_processing* PP,
                                            char* GUID, 
                                            u32 EffectType)
{
    Assert(PP->NumEffects < POST_PROC_MAX_EFFECTS);
    post_proc_effect* Effect = &PP->Effects[PP->NumEffects++];
    
    Effect->EffectType = EffectType;
    Effect->Enabled = true;
    CopyStringsSafe(Effect->GUID, ArrLen(Effect->GUID), GUID);
    
    PostProcEffect_DefaultParams(&Effect->Params, EffectType);
    
    return Effect;
}

INTERNAL_FUNCTION void InitSSAO(post_processing* PP)
{
    // NOTE(Dima): Init SSAO kernel
    for(int i = 0; i < ArrLen(PP->SSAO_Kernel); i++)
    {
        v3 RandomVector = NOZ(V3(RandomBilateral(&PP->Random),
                                 RandomBilateral(&PP->Random),
                                 RandomUnilateral(&PP->Random)));
        
        f32 Scale = RandomUnilateral(&PP->Random);
        Scale = Lerp(0.1f, 1.0f, Scale * Scale);
        
        PP->SSAO_Kernel[i] = RandomVector * Scale;
    }
    
    // NOTE(Dima): Init SSAO noise
    for(int i = 0; i < ArrLen(PP->SSAO_Noise); i++)
    {
        
        PP->SSAO_Noise[i] = Normalize(V3(RandomBilateral(&PP->Random), 
                                         RandomBilateral(&PP->Random), 
                                         RandomBilateral(&PP->Random)));
    }
}

INTERNAL_FUNCTION void InitPostprocessing(post_processing* PP, memory_arena* Arena)
{
    PP->Random = SeedRandom(62313);
    PP->Arena = Arena;
    
    InitSSAO(PP);
    
    // NOTE(Dima): Add standard post proc effects
    post_proc_effect* MainSSAO = PostProcEffect_Add(PP, "MainSSAO", 
                                                    PostProcEffect_SSAO);
    
    post_proc_effect* InWaterSSAO = PostProcEffect_Add(PP, "InWaterSSAO", 
                                                       PostProcEffect_SSAO);
    
    
    post_proc_effect* MainDOF = PostProcEffect_Add(PP, "MainDOF", 
                                                   PostProcEffect_DOF);
    
    post_proc_effect* MainDOFBlur = PostProcEffect_Add(PP, "MainDOF_Blur", 
                                                       PostProcEffect_BoxBlur);
    
    post_proc_effect* CrtDisplay = PostProcEffect_Add(PP, "CrtDisplay",
                                                      PostProcEffect_CrtDisplay);
    
    post_proc_effect* Posterize = PostProcEffect_Add(PP, "Posterize",
                                                     PostProcEffect_Posterize);
    
    // NOTE(Dima): Init precomputed 1d gaussian kernels
    for (int Radius = 0; 
         Radius < ArrLen(PP->Gaussian1DKernelForRadius); 
         Radius++)
    {
        f32* Kernel = PushArray(Arena, f32, Radius + 1);
        
        GaussianKernelGenerate1D(Kernel, Radius);
        
        PP->Gaussian1DKernelForRadius[Radius] = Kernel;
    }
}