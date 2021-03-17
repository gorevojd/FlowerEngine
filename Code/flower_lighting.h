#ifndef FLOWER_LIGHTING_H
#define FLOWER_LIGHTING_H

struct directional_light
{
    v3 Dir;
    v3 C;
};

struct point_light
{
    v3 P;
    v3 C;
    f32 Radius;
};

struct lighting
{
    random_generation Random;
    
    directional_light DirLit;
};

#endif //FLOWER_LIGHTING_H
