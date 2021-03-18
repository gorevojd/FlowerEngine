#version 330 core

in vec2 FragUV;
out vec4 Color;

uniform sampler2D DepthTex;
uniform sampler2D NormalTex;
uniform sampler2D ColorSpecTex;
uniform sampler2D SSAOTex;
uniform sampler2D PositionsTex;
uniform sampler2D ViewPositionsTex;
uniform vec4 PerspProjCoefs;
uniform vec2 WH;

uniform vec3 CameraP;
uniform vec3 DirectionalLightDirection;
uniform vec3 DirectionalLightColor;

uniform samplerCube Sky;
uniform bool SkyIsSet;

uniform bool SSAOEnabled;

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

void main()
{
	vec2 PixelDelta = vec2(1.0f) / WH;

	vec4 SampleColorSpec = texture2D(ColorSpecTex, FragUV);
	vec3 Normal = texture2D(NormalTex, FragUV).xyz;
	vec3 SamplePosition = texture2D(PositionsTex, FragUV).xyz;
	vec3 SampleViewPosition = texture2D(ViewPositionsTex, FragUV).xyz;
	vec3 FragWorldP = SamplePosition;

	vec3 ResultColor = vec3(0.0f);
	ResultColor += SampleColorSpec.rgb * 0.25f;
	ResultColor += CalcDirLit(SampleColorSpec.rgb, Normal);
	
	if(SSAOEnabled)
	{
		float SampleOcclusion = texture2D(SSAOTex, FragUV).r;
		ResultColor *= SampleOcclusion;
	}
	
	Color = vec4(ResultColor, 1.0f);
	
	//Color = vec4(Normal * 0.5f + vec3(0.5f), 1.0f);
	//Color = vec4(ViewNormal * 0.5f + vec3(0.5f), 1.0f);
	//Color = vec4(vec3(texture2D(SSAOTex, FragUV).r), 1.0f);
	//Color = vec4(vec3(SampleViewPosition.z / 1500.0f), 1.0f);
	//Color = vec4(vec3(GetLinearizedDepth(FragUV) / 1500.0f), 1.0f);
	//Color = vec4(SamplePosition, 1.0f);
	//Color = vec4(SampleColorSpec.rgb, 1.0f);
	//Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
