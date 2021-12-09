#version 330 core

in vec2 FragUV;
out float OutOcclusion;

uniform sampler2D OcclusionTex;
uniform int BlurRadius;
uniform float GaussianKernel[32];
uniform bool IsHorizontalPass;

void main()
{
	vec2 PixelSize = vec2(1.0f) / textureSize(OcclusionTex, 0);
	float SumOcclusion = 0.0f;

	for(int i = -BlurRadius; i <= BlurRadius; i++)
	{
		vec2 OffsetUV = vec2(0.0, 0.0);
		if (IsHorizontalPass)
		{
			OffsetUV.x = i;
		}
		else
		{
			OffsetUV.y = i;
		}

		float Occlusion = texture2D(OcclusionTex, FragUV + OffsetUV * PixelSize).r;
		Occlusion *= GaussianKernel[abs(i)];

		SumOcclusion += Occlusion;
	}
	
	OutOcclusion = SumOcclusion;
}