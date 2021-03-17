INTERNAL_FUNCTION void InitLighting(lighting* Lighting)
{
    Lighting->DirLit.Dir = NOZ(V3(-0.5f, -0.5f, -0.8f));
    Lighting->DirLit.C = V3(1.0f, 1.0f, 1.0f);
}