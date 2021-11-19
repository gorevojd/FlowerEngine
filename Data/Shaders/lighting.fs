#version 330 core

in vec2 FragUV;
out vec4 Color;

uniform sampler2D DepthTex;
uniform sampler2D NormalTex;
uniform sampler2D ColorSpecTex;
uniform sampler2D SSAOTex;
uniform sampler2D PositionsTex;
uniform vec4 PerspProjCoefs;
uniform vec2 WH;

uniform vec3 CameraP;
uniform float AmbientPercentage;
uniform float ShadowStrength;

uniform vec3 DirectionalLightDirection;
uniform vec3 DirectionalLightColor;
uniform bool CalculateDirLightShadow;
uniform sampler2DArray LightDepthTex;
uniform float CascadeDistances[8];
uniform mat4 CascadeLightProjections[8];
uniform int CascadeCount;

uniform vec2 PoissonSamples[12];
uniform sampler2D PoissonSamplesRotations;
uniform bool ShouldRotateSamples;

uniform bool SSAOEnabled;
uniform float SSAOContribution;

vec3 CalcDirLit(vec3 FragSampleC, vec3 FragN)
{
	float DiffCoef = max(dot(FragN, -DirectionalLightDirection), 0.0f);
	vec3 Result = DiffCoef * DirectionalLightColor * FragSampleC;

	return(Result); 
}

float GetLinearizedDepth(vec2 UV)
{
	float SampleDepth = texture2D(DepthTex, UV).r;
	float Zndc = SampleDepth * 2.0f - 1.0f;
	float LinearDepth = PerspProjCoefs.w / (Zndc - PerspProjCoefs.z);

	return(LinearDepth);
}

float GetLinearizedDepth(float SampleDepth)
{
	float Zndc = SampleDepth * 2.0f - 1.0f;
	float LinearDepth = PerspProjCoefs.w / (Zndc - PerspProjCoefs.z);

	return(LinearDepth);
}

float LinStep(float Value, float Min, float Max)
{
	float Result = clamp((Value - Min) / (Max - Min), 0, 1);

	return(Result);
}

float ReduceLightBleeding(float PMax, float Amount)
{
	float Result = LinStep(PMax, Amount, 1);

	return(Result);
}

float GetShadowFromDepthAndUV(vec3 WorldP, int SampleLayerIndex)
{
	// Projecting fragment to light space
	vec4 FragLitProjected = vec4(WorldP, 1.0) * CascadeLightProjections[SampleLayerIndex];
	FragLitProjected /= FragLitProjected.w;
	FragLitProjected.xyz = FragLitProjected.xyz * 0.5 + 0.5;

	float FragDepth = FragLitProjected.z;
	vec3 SampleDepthUV = vec3(FragLitProjected.xy, SampleLayerIndex);	

	float M1 = texture(LightDepthTex, SampleDepthUV).r;
	float M2 = texture(LightDepthTex, SampleDepthUV).g;
	float ClosestDepth = M1;

	float p = step(FragDepth, M1);
	float Nu = M1;
	float SigmaSquared = max(M2 - M1 * M1, 0.0000002);
	float Temp = FragDepth - Nu;
	float PMaxT = SigmaSquared / (SigmaSquared + Temp * Temp);
	PMaxT = clamp(max(p, PMaxT), 0, 1);
	PMaxT = ReduceLightBleeding(PMaxT, 0.2);

	float Shadow = 1.0 - PMaxT;

	return Shadow;
}

bool GetLerpCoeffBetweenCascades(float LinearDepth, int CascadeIndex,
									out int Index1, out int Index2,
									out float LerpCoeff)
{
	bool Result = false;
	
	float CascadeDist = CascadeDistances[CascadeIndex];

	float LerpDistBias = CascadeDist * 0.1;

	float Diff = LinearDepth - CascadeDist;
	
	if (abs(Diff) < LerpDistBias)
	{
		Index1 = CascadeIndex;
		Index2 = CascadeIndex + 1;

		float a = LinearDepth - (CascadeDist - LerpDistBias);
		LerpCoeff = a / (2.0 * LerpDistBias);

		Result = true;
	}

	return Result;
}

float ShadowCalculation(vec3 WorldP, vec3 WorldN, float LinearDepth)
{
	vec3 ShadowTexSize = textureSize(LightDepthTex, 0);

	int SampleLayerIndex1 = -1;
	int SampleLayerIndex2 = -1;
	float LerpCoeff = 0.0;

	for(int CascadeIndex = 0;
		CascadeIndex < CascadeCount;
		CascadeIndex++)
	{
		if(LinearDepth < CascadeDistances[CascadeIndex])
		{		
			if (CascadeIndex > 0)
			{
				int Index1;
				int Index2;
				if(GetLerpCoeffBetweenCascades(LinearDepth, CascadeIndex - 1,
											   Index1, Index2,
											   LerpCoeff))
				{
					SampleLayerIndex1 = Index1;
					SampleLayerIndex2 = Index2;

					break;
				}
			}
/*
*/
			if (CascadeIndex + 1 < CascadeCount)
			{
				int Index1;
				int Index2;
				if(GetLerpCoeffBetweenCascades(LinearDepth, CascadeIndex, 
											   Index1, Index2,
											   LerpCoeff))
				{
					SampleLayerIndex1 = Index1;
					SampleLayerIndex2 = Index2;

					break;
				}
			}

			LerpCoeff = 0.0;
			SampleLayerIndex1 = CascadeIndex;
			SampleLayerIndex2 = -1;

			break;
		}
	}

	// Getting shadow of first cascade
	float Shadow1 = GetShadowFromDepthAndUV(WorldP, SampleLayerIndex1);
	
	// Getting shadow of second cascade
	float Shadow2 = 0.0;
	if (SampleLayerIndex2 != -1)
	{
		Shadow2 = GetShadowFromDepthAndUV(WorldP, SampleLayerIndex2);
	}

	// Mixing shadows of 2 neighbour cascades
	float Shadow = mix(Shadow1, Shadow2, LerpCoeff);

	return(Shadow);
}

void main()
{	
	vec2 PixelDelta = vec2(1.0) / WH;

	vec4 SampleColorSpec = texture2D(ColorSpecTex, FragUV);
	float SampleDepth = texture2D(DepthTex, FragUV).r;

	vec3 ResultColor;
	if(SampleDepth < 0.9999999)
	{
		vec3 WorldN = texture2D(NormalTex, FragUV).xyz;
		vec3 WorldP = texture2D(PositionsTex, FragUV).xyz;
		
		float TotalShadow = 0.0f;
		if(CalculateDirLightShadow)
		{
			float LinearDepth = GetLinearizedDepth(SampleDepth);
			
			TotalShadow = ShadowCalculation(WorldP, WorldN, LinearDepth);
		}
		
		if(SSAOEnabled)
		{
			float SampleOcclusion = (1.0 - texture2D(SSAOTex, FragUV).r) * SSAOContribution;

			TotalShadow += SampleOcclusion;
		}

		TotalShadow = clamp(1.0 - TotalShadow, 0, 1);

		ResultColor += SampleColorSpec.rgb * AmbientPercentage;
		ResultColor += CalcDirLit(SampleColorSpec.rgb, WorldN) * TotalShadow;
	}
	else
	{
		ResultColor = SampleColorSpec.rgb;
	}
	
	//ResultColor = SampleColorSpec.rgb;

	Color = vec4(ResultColor, 1.0);

	//Color = vec4(vec3(texture2D(SSAOTex, FragUV).r), 1.0f);
	//Color = vec4(vec3(GetLinearizedDepth(FragUV) / 1500.0f), 1.0f);
	//Color = vec4(SamplePosition, 1.0f);
	//Color = vec4(SampleColorSpec.rgb, 1.0f);
}
