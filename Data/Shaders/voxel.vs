#version 330 core

layout (location = 0) in uint InPosition;

out vs_out 
{
	flat vec3 Color;
	flat vec3 Normal;
	vec2 TexCoords;
	vec3 FragP;
} VsOut;

vec3 NormalTypes[6] = vec3[6](
	vec3(0.0f, 0.0f, 1.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(1.0f, 0.0f, 0.0f),
	vec3(-1.0f, 0.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, -1.0f, 0.0f)
);

uniform mat4 ViewProjection;
uniform mat4 Projection;
uniform mat4 View;

uniform vec3 ChunkAt;
uniform usamplerBuffer PerFaceData;

void main()
{
	//Extracting vertex data
	uint InChunkX = InPosition & 63u;
	uint InChunkZ = (InPosition >> 6u) & 63u;
	uint InChunkY = (InPosition >> 12u) & 255u;

	//Extracting per face data
	uint PerFaceEntry = texelFetch(PerFaceData, gl_VertexID / 6).r;
	
	//3 bits for normals
	uint NormalType = PerFaceEntry & 7u;
	VsOut.Normal = NormalTypes[NormalType];

	//8 bits for bitmap ID
	float DeltaUV = 1.0f / 16.0f;
	uint BitmapID = (PerFaceEntry >> 3u) & 255u;
	
	vec2 StartUV_ = vec2(float(BitmapID % 16u), float(BitmapID / 16u)) * DeltaUV;
	vec2 EndUV_ = StartUV_ + vec2(DeltaUV);

	vec2 SmallOffset = vec2(DeltaUV) * 0.01f;

	vec2 StartUV = StartUV_ + SmallOffset;
	vec2 EndUV = EndUV_ - SmallOffset;

	//Getting texture coordinates
	int InFaceID = gl_VertexID % 6;
	if ((InFaceID == 0) || (InFaceID == 3))
	{
		VsOut.TexCoords = StartUV;
	}
	else if(InFaceID == 1)
	{
		VsOut.TexCoords = vec2(EndUV.x, StartUV.y);
	}
	else if(InFaceID == 5)
	{
		VsOut.TexCoords = vec2(StartUV.x, EndUV.y);
	}
	else if((InFaceID == 2) || (InFaceID == 4))
	{
		VsOut.TexCoords = EndUV;
	}

	//Calculating vertex clip space P
	vec3 VertP = ChunkAt + vec3(float(InChunkX), float(InChunkY), float(InChunkZ));
	vec4 CalculatedP = vec4(VertP, 1.0f);

	gl_Position = CalculatedP * ViewProjection;

	VsOut.FragP = VertP;
	VsOut.Color = vec3(1.0f);
}