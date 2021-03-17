#version 330 core

in vec2 FragUV;
out float OutOcclusion;

uniform sampler2D OcclusionTex;
uniform int BlurRadius;

void main()
{
	vec2 PixelSize = vec2(1.0f) / textureSize(OcclusionTex, 0);

	float SumOcclusion = 0.0f;
	for(int j = -BlurRadius; j <= BlurRadius; j++)
	{
		for(int i = -BlurRadius; i <= BlurRadius; i++)
		{
			float Occlusion = texture2D(OcclusionTex, FragUV + vec2(i, j) * PixelSize).r;

			SumOcclusion += Occlusion;
		}
	}

	SumOcclusion /= (BlurRadius * 2 + 1) * (BlurRadius * 2 + 1);

	OutOcclusion = SumOcclusion;
}