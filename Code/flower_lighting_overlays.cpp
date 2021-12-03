INTERNAL_FUNCTION
void Lighting_ShowShadowsRes(lighting* Lighting)
{
    BeginRow();
    
    ShowTextUnformatted("ShadowMap res: ");
    
    int CurRes = Lighting->ShadowMapRes;
    
    if (Button("512", CurRes == 512))
    {
        Lighting->ShadowMapRes = 512;
    }
    
    if (Button("1024", CurRes == 1024))
    {
        Lighting->ShadowMapRes = 1024;
    }
    
    if (Button("2048", CurRes == 2048))
    {
        Lighting->ShadowMapRes = 2048;
    }
    
    if (CurRes != Lighting->ShadowMapRes)
    {
        Lighting->NeedResettingShadowMaps = true;
    }
    
    EndRow();
}

INTERNAL_FUNCTION
void Lighting_ShowShadowsOverlay(lighting* Lighting)
{
    
    SliderFloat("AmbientPercentage", 
                &Lighting->AmbientPercentage,
                0.0f, 1.0f);
    
    Lighting_ShowShadowsRes(Lighting);
    BoolButton("BlurShadowMaps", &Lighting->BlurVarianceShadowMaps);
    SliderInt("BlurRadius", 
              &Lighting->VarianceShadowMapBlurRadius,
              1, 4);
    SliderFloat("ShadowStrength", &Lighting->ShadowStrength, 0.0f, 1.0f);
    
}

INTERNAL_FUNCTION
void Lighting_ShowOverlays(lighting* Lighting)
{
    if (TreeNode("Lighting"))
    {
        Lighting_ShowShadowsOverlay(Lighting);
        
        TreePop();
    }
}