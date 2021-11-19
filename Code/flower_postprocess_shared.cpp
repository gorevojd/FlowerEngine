inline post_proc_effect* GetPostProcEffectByGUID(post_processing* PP, char* GUID)
{
    post_proc_effect* Result = 0;
    
    for (int EffectIndex = 0;
         EffectIndex < PP->NumEffects;
         EffectIndex++)
    {
        post_proc_effect* Effect = &PP->Effects[EffectIndex];
        
        if(StringsAreEqual(GUID, Effect->GUID))
        {
            Result = Effect;
            break;
        }
    }
    
    return Result;
}

inline void* PostProcEffect_GetParams_(post_processing* PP, char* GUID)
{
    post_proc_effect* Effect = GetPostProcEffectByGUID(PP, GUID);
    Assert(Effect);
    
    void* Result = 0;
    
    if(Effect)
    {
        Result = &Effect->Params.Union;
    }
    
    return Result;
}

#define PostProcEffect_GetParams(pp, guid, type) (type*)PostProcEffect_GetParams_(pp, guid)

inline b32 PostProcEffect_IsEnabled(post_processing* PP, char* GUID)
{
    b32 Result = false;
    
    post_proc_effect* Effect = GetPostProcEffectByGUID(PP, GUID);
    Assert(Effect);
    
    if (Effect)
    {
        Result = Effect->Enabled;
    }
    
    return Result;
}

inline void PostProcEffect_SetEnabled(post_processing* PP, char* GUID, b32 Enabled)
{
    post_proc_effect* Effect = GetPostProcEffectByGUID(PP, GUID);
    Assert(Effect);
    
    if(Effect)
    {
        Effect->Enabled = Enabled;
    }
}