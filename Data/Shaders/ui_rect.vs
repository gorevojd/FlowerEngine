#version 330 core

layout (location = 0) in vec4 InPosUV;

out vec2 TexCoords;
out vec4 Color;
flat out int TextureIndex;

uniform bool IsBatch;
uniform vec4 MultColor;
uniform usamplerBuffer RectsColors;
uniform usamplerBuffer RectsTextureIndices;

uniform mat4 ViewProjection;

void main()
{
	int PerRectIndex = gl_VertexID / 4;

	//NOTE(Dima): Calculating Clip-Space position
	gl_Position = vec4(InPosUV.xy, 0.0f, 1.0f) * ViewProjection;

	//NOTE(Dima): Extracting color
	vec4 ExtractedColor = MultColor;
	if(IsBatch)
	{
		uint InColor = texelFetch(RectsColors, PerRectIndex).r;

		float OneOver255 = 1.0f / 255.0f;
		float InColorR = float((InColor >> 0u) & 255u) * OneOver255;
		float InColorG = float((InColor >> 8u) & 255u) * OneOver255;
		float InColorB = float((InColor >> 16u) & 255u) * OneOver255;
		float InColorA = float((InColor >> 24u) & 255u) * OneOver255;

		ExtractedColor = vec4(InColorR, InColorG, InColorB, InColorA);

		//NOTE(dima): Fetching rect texture index
		uint RectTextureIndex = texelFetch(RectsTextureIndices, PerRectIndex).r;
		TextureIndex = int(RectTextureIndex);
	}
	else
	{
		TextureIndex = 0;
	}
	
	Color = ExtractedColor;
	TexCoords = InPosUV.zw;
}


























