#ifndef FLOWER_POSTPROCESS_H
#define FLOWER_POSTPROCESS_H

enum post_proc_resolution
{
    PostProcResolution_Normal,
    PostProcResolution_Half,
    PostProcResolution_Quater,
    
    PostProcResolution_Count,
};

GLOBAL_VARIABLE int Global_PostProcResolutionDivisors[PostProcResolution_Count] =
{
    1,
    2,
    4,
};

enum post_proc_effect_type
{
    PostProcEffect_Posterize,
    PostProcEffect_BoxBlur,
    PostProcEffect_Dilation,
    PostProcEffect_DOF,
    PostProcEffect_SSAO,
};

struct posterize_params
{
    int Levels;
    
    post_proc_resolution Resolution;
};

struct box_blur_params
{
    int RadiusSize;
    post_proc_resolution Resolution;
    b32 EnableCheapMode;
};

struct dilation_params
{
    int Size;
    
    f32 MinThreshold;
    f32 MaxThreshold;
    
    post_proc_resolution Resolution;
};

struct dof_params
{
    f32 MinDistance;
    f32 MaxDistance;
    
    f32 FocusZ;
};

struct ssao_params
{
    int KernelSize;
    f32 KernelRadius;
    f32 Contribution;
    f32 RangeCheck;
    
    b32 BlurEnabled;
    int BlurRadius;
    
    post_proc_resolution Resolution;
};

struct post_proc_params
{
    union
    {
        posterize_params Posterize;
        box_blur_params BoxBlur;
        dilation_params Dilation;
        dof_params DOF;
        ssao_params SSAO;
    } Union;
};

struct post_proc_effect
{
#define POST_PROC_EFFECT_GUID_SIZE 128
    char GUID[POST_PROC_EFFECT_GUID_SIZE];
    
    u32 EffectType;
    
    b32 Enabled;
    post_proc_params Params;
};

inline void PostProcEffect_DefaultParams(post_proc_params* ParamsBase, u32 EffectType)
{
    switch(EffectType)
    {
        case PostProcEffect_Posterize:
        {
            posterize_params* Posterize = &ParamsBase->Union.Posterize;
            
            Posterize->Levels = 7;
            Posterize->Resolution = PostProcResolution_Normal;
        }break;
        
        case PostProcEffect_BoxBlur:
        {
            box_blur_params* BoxBlur = &ParamsBase->Union.BoxBlur;
            
            BoxBlur->RadiusSize = 2;
            BoxBlur->EnableCheapMode = false;
            BoxBlur->Resolution = PostProcResolution_Normal;
        }break;
        
        case PostProcEffect_Dilation:
        {
            dilation_params* Dilation = &ParamsBase->Union.Dilation;
            
            Dilation->Size = 2;
            Dilation->MinThreshold = 0.1f;
            Dilation->MaxThreshold = 0.3f;
            Dilation->Resolution = PostProcResolution_Normal;
        }break;
        
        case PostProcEffect_DOF:
        {
            dof_params* DOF = &ParamsBase->Union.DOF;
            
            DOF->MinDistance = 300.0f;
            DOF->MaxDistance = 1200.0f;
            DOF->FocusZ = 0.0f;
        }break;
        
        case PostProcEffect_SSAO:
        {
            ssao_params* SSAO = &ParamsBase->Union.SSAO;
            
            SSAO->KernelSize = 64;
            
            SSAO->KernelRadius = 0.6f;
            SSAO->Contribution = 1.0f;
            SSAO->RangeCheck = 0.25f;
            SSAO->BlurRadius = 2;
            SSAO->Resolution = PostProcResolution_Normal;
        }break;
    }
}

struct post_processing
{
    random_generation Random;
    
    v3 SSAO_Kernel[128];
    v3 SSAO_Noise[16];
    
#define POST_PROC_MAX_EFFECTS 32
    post_proc_effect Effects[POST_PROC_MAX_EFFECTS];
    int NumEffects;
};

#endif //FLOWER_POSTPROCESS_H
