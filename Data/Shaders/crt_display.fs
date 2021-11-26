#version 330 core

in vec2 FragUV;
out vec3 OutColor;

uniform vec2 Curvature;
uniform vec2 DstTextureSize;
uniform vec2 ScanLineOpacity;
uniform float CellSize;
uniform sampler2D InputTexture;

vec2 CurveRemapUV_CRT(vec2 uv)
{
	uv = uv * 2.0 - 1.0;
	vec2 offset = abs(uv.yx) / vec2(Curvature.x, Curvature.y);
	uv = uv + uv * offset * offset;
	uv = uv * 0.5 + 0.5;

	return uv;
}

vec3 ScanLineIntensity(float Coord, float Resolution, float Opacity)
{
	float Intensity = sin(Coord * Resolution * 3.1415926 * 2.0 / CellSize);
	
	Intensity = 0.5 * Intensity + 0.5;

	Intensity = 0.1 + Intensity * 0.9;

	float ResultValue = pow(Intensity, Opacity);
	return vec3(ResultValue);
}

void main()
{
	vec2 RemappedUV = CurveRemapUV_CRT(FragUV);

	vec3 BaseColor = texture2D(InputTexture, RemappedUV).rgb;

	BaseColor *= ScanLineIntensity(RemappedUV.x, DstTextureSize.x, ScanLineOpacity.x);
	BaseColor *= ScanLineIntensity(RemappedUV.y, DstTextureSize.y, ScanLineOpacity.y);

	if (RemappedUV.x < 0.0 || RemappedUV.x > 1.0 || 
		RemappedUV.y < 0.0 || RemappedUV.y > 1.0)
	{
		OutColor = vec3(0.0);
	}
	else
	{
		OutColor = BaseColor.rgb;
	}
}