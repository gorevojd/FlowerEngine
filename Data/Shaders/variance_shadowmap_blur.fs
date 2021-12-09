#version 330 core

in vec2 FragUV;
out vec2 ResultColor;

uniform sampler2DArray TextureArray;
uniform int TextureIndex;
uniform int BlurRadius;
uniform bool IsHorizontalPass;
uniform float GaussianKernel[32];

void main()
{
	vec2 PixelSize = vec2(1.0f) / textureSize(TextureArray, 0).xy;

	vec2 BlurredSum = vec2(0.0);

	for(int i = -BlurRadius; i <= BlurRadius; i++)
	{
		vec2 OffsetUV = vec2(0.0);
		if (IsHorizontalPass)
		{
			OffsetUV.x = i;
		}
		else
		{
			OffsetUV.y = i;
		}

		vec3 SampleUV = vec3(FragUV + OffsetUV * PixelSize, TextureIndex);
		vec2 SampleDepth = texture(TextureArray, SampleUV).rg;
		SampleDepth *= GaussianKernel[abs(i)];

		BlurredSum += SampleDepth;
	}

	ResultColor = BlurredSum;
}