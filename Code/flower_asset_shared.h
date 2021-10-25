#ifndef FLOWER_ASSET_SHARED_H
#define FLOWER_ASSET_SHARED_H


// Materials
enum material_type
{
    Material_SpecularDiffuse,
    Material_Solid,
    Material_PBR,
};

enum material_textures_specular_diffuse
{
    MatTex_SpecularDiffuse_Diffuse,
    MatTex_SpecularDiffuse_Normal,
    MatTex_SpecularDiffuse_Specular,
    
    MatTex_SpecularDiffuse_Count,
};

enum material_textures_pbr
{
    MatTex_PBR_Albedo,
    MatTex_PBR_Normal,
    MatTex_PBR_Metallic,
    MatTex_PBR_Roughness,
    
    MatTex_PBR_Count,
};

#define MAX_MATERIAL_TEXTURES 16


// Animations
enum animation_ouside_behaviour
{
    AnimOutsideBehaviour_Closest,
    AnimOutsideBehaviour_Repeat,
};


#endif //FLOWER_ASSET_TYPES_SHARED_H
