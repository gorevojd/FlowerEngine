#ifndef FLOWER_POSTPROCESS_H
#define FLOWER_POSTPROCESS_H

struct pp_dilation_params
{
    int Size;
    
    f32 MinThreshold;
    f32 MaxThreshold;
};

struct pp_ssao_params
{
    int KernelSize;
    f32 KernelRadius;
    f32 Contribution;
    f32 RangeCheck;
    int BlurRadius;
    
    b32 Enabled;
};

inline pp_ssao_params PP_SSAO_DefaultParams()
{
    pp_ssao_params Result = {};
    
    Result.KernelSize = 64;
    Result.KernelRadius = 1.0f;
    //Result.Contribution = 1.0f;
    Result.Contribution = 2.0f;
    Result.RangeCheck = 0.25f;
    Result.BlurRadius = 2;
    
    Result.Enabled = true;
    
    return(Result);
}

inline pp_dilation_params PP_DilationDefaultParams()
{
    pp_dilation_params Result = {};
    
    Result.Size = 5;
    Result.MinThreshold = 0.1f;
    Result.MaxThreshold = 0.3f;
    
    return(Result);
}

struct postprocessing
{
    random_generation Random;
    
    v3 SSAO_Kernel[128];
    v3 SSAO_Noise[16];
    pp_ssao_params SSAO_Params;
};

#endif //FLOWER_POSTPROCESS_H
