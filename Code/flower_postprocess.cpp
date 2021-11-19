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

INTERNAL_FUNCTION void InitPostprocessing(post_processing* PP)
{
    PP->Random = SeedRandom(62313);
    
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
}