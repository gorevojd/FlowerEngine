#ifndef FLOWER_POSTPROCESS_H
#define FLOWER_POSTPROCESS_H

GLOBAL_VARIABLE int Global_PostProcResolutionDivisors[FramebufPoolType_Count] =
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
    PostProcEffect_CrtDisplay,
};

struct posterize_params
{
    int Levels;
    
    framebuffer_pool_resolution Resolution;
};

struct box_blur_params
{
    int RadiusSize;
    b32 EnableCheapMode;
    framebuffer_pool_resolution Resolution;
};

struct dilation_params
{
    int Size;
    
    f32 MinThreshold;
    f32 MaxThreshold;
    
    framebuffer_pool_resolution Resolution;
};

struct crt_display_params
{
    v2 Curvature;
    v2 ScanLineOpacity;
    f32 CellSize;
    
    // NOTE(Dima): Vignette related stuff
    b32 VignetteEnabled;
    f32 VignetteRoundness;
    f32 VignetteOpacity;
    f32 VignetteBrightnessCompensation;
    f32 VignettePower;
    
    framebuffer_pool_resolution Resolution;
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
    
    framebuffer_pool_resolution Resolution;
    framebuffer_pool_resolution BlurResolution;
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
        crt_display_params CrtDisplay;
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
            Posterize->Resolution = FramebufPoolRes_Normal;
        }break;
        
        case PostProcEffect_BoxBlur:
        {
            box_blur_params* BoxBlur = &ParamsBase->Union.BoxBlur;
            
            BoxBlur->RadiusSize = 2;
            BoxBlur->EnableCheapMode = false;
            BoxBlur->Resolution = FramebufPoolRes_Normal;
        }break;
        
        case PostProcEffect_Dilation:
        {
            dilation_params* Dilation = &ParamsBase->Union.Dilation;
            
            Dilation->Size = 2;
            Dilation->MinThreshold = 0.1f;
            Dilation->MaxThreshold = 0.3f;
            Dilation->Resolution = FramebufPoolRes_Normal;
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
            SSAO->BlurEnabled = true;
            
            framebuffer_pool_resolution Res = FramebufPoolRes_Normal;
            SSAO->Resolution = Res;
            SSAO->BlurResolution = Res;
        }break;
        
        case PostProcEffect_CrtDisplay:
        {
            crt_display_params* Params = &ParamsBase->Union.CrtDisplay;
            
            Params->Curvature = V2(4.0f, 3.4f);
            Params->ScanLineOpacity = V2(0.1f, 0.16f);
            Params->CellSize = 7.0f;
            Params->VignetteEnabled = true;
            Params->VignetteRoundness = 250.0f;
            Params->VignetteOpacity = 1.0f;
            Params->VignettePower = 0.5f;
            Params->VignetteBrightnessCompensation = 1.3f;
            
            Params->Resolution = FramebufPoolRes_Normal;
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