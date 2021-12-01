INTERNAL_FUNCTION
void DisplayResolutionRadioButton(post_proc_resolution* Resolution,
                                  char* Name)
{
    ui_params* Params = UIGetParams();
    
    BeginRow();
    
    ShowTextUnformatted(Name);
    
    if(Button("1x"))
    {
        *Resolution = PostProcResolution_Normal;
    }
    
    if(Button("0.5x"))
    {
        *Resolution = PostProcResolution_Half;
    }
    
    if(Button("0.25x"))
    {
        *Resolution = PostProcResolution_Quater;
    }
    
    EndRow();
}

INTERNAL_FUNCTION
void DisplayParams_Posterize(posterize_params* Params)
{
    DisplayResolutionRadioButton(&Params->Resolution, "Resolution");
    
    
}

INTERNAL_FUNCTION
void DisplayParams_BoxBlur(box_blur_params* Params)
{
    DisplayResolutionRadioButton(&Params->Resolution, "Resolution");
}

INTERNAL_FUNCTION
void DisplayParams_Dilation(dilation_params* Params)
{
    DisplayResolutionRadioButton(&Params->Resolution, "Resolution");
}

INTERNAL_FUNCTION
void DisplayParams_DOF(dof_params* Params)
{
    SliderFloat("MinDistance", &Params->MinDistance, 0, 400);
    SliderFloat("MaxDistance", &Params->MaxDistance, 400, 1500);
    
    SliderFloat("FocusZ", &Params->FocusZ, 0, 500);
}

INTERNAL_FUNCTION
void DisplayParams_SSAO(ssao_params* Params)
{
    DisplayResolutionRadioButton(&Params->Resolution, "Resolution");
}

INTERNAL_FUNCTION
void DisplayParams_CrtDisplay(crt_display_params* Params)
{
    DisplayResolutionRadioButton(&Params->Resolution, "Resolution");
    
    // NOTE(Dima): Some params
    SliderFloat("CurvatureX", &Params->Curvature.x, 0.0f, 5.0f);
    SliderFloat("CurvatureY", &Params->Curvature.y, 0.0f, 5.0f);
    
    SliderFloat("ScanLineOpacityX", &Params->ScanLineOpacity.x, 0.0f, 1.0f);
    SliderFloat("ScanLineOpacityY", &Params->ScanLineOpacity.y, 0.0f, 1.0f);
    
    SliderFloat("CellSize", &Params->CellSize, 1.5f, 15.0f);
    
    // NOTE(Dima): Vignette params
    BoolButton("VignetteEnabled",
               &Params->VignetteEnabled);
    SliderFloat("VignetteRoundness",
                &Params->VignetteRoundness,
                0.0f, 1.0f);
    SliderFloat("VignetteOpacity", 
                &Params->VignetteOpacity,
                0.0f, 1.0f);
    SliderFloat("VignetteBrightnessCompensation",
                &Params->VignetteBrightnessCompensation,
                0.0f, 5.0f);
    SliderFloat("VignettePower",
                &Params->VignettePower,
                0.0f, 2.0f);
}

INTERNAL_FUNCTION
void PostProcess_DisplayEffect(post_proc_effect* Effect)
{
    // NOTE(Dima): Display tree node of post process effect
    if(TreeNode(Effect->GUID))
    {
        // NOTE(Dima): Displaying params of post-processing effect
        switch (Effect->EffectType)
        {
            case PostProcEffect_Posterize:
            {
                posterize_params* Params = &Effect->Params.Union.Posterize;
                
                DisplayParams_Posterize(Params);
            }break;
            
            case PostProcEffect_BoxBlur:
            {
                box_blur_params* Params = &Effect->Params.Union.BoxBlur;
                
                DisplayParams_BoxBlur(Params);
            }break;
            
            case PostProcEffect_Dilation:
            {
                dilation_params* Params = &Effect->Params.Union.Dilation;
                
                DisplayParams_Dilation(Params);
            }break;
            
            case PostProcEffect_DOF:
            {
                dof_params* Params = &Effect->Params.Union.DOF;
                
                DisplayParams_DOF(Params);
            }break;
            
            case PostProcEffect_SSAO:
            {
                ssao_params* Params = &Effect->Params.Union.SSAO;
                
                DisplayParams_SSAO(Params);
            }break;
            
            case PostProcEffect_CrtDisplay:
            {
                crt_display_params* Params = &Effect->Params.Union.CrtDisplay;
                
                DisplayParams_CrtDisplay(Params);
            }break;
        }
        
        TreePop();
    }
}

INTERNAL_FUNCTION
void PostProcess_ShowOverlays(post_processing* PP)
{
    if (TreeNode("Post-processing"))
    {
        for (int EffectIndex = 0;
             EffectIndex < PP->NumEffects;
             EffectIndex++)
        {
            post_proc_effect* Effect = &PP->Effects[EffectIndex];
            
            PostProcess_DisplayEffect(Effect);
        }
        
        TreePop();
    }
}