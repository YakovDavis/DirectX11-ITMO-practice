#pragma pack_matrix(row_major)

struct VS_IN
{
	float4 pos : POSITION0;
	float4 tex : TEXCOORD0;
	float4 normal : NORMAL0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
 	float4 tex : TEXCOORD;
	float4 normal : NORMAL;
	float4 worldViewPos : WORLDPOS;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
	float4x4 gWorldView;
	float4x4 gInvTrWorldView;
};

cbuffer cbPerScene : register(b1)
{
	float4 lightPos;
	float4 lightColor;
	float4 ambientSpecularPowType;
};

Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0);

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;
	
	output.pos = mul(float4(input.pos.xyz, 1.0f), gWorldViewProj);
	output.tex = input.tex;
	output.normal = mul(float4(input.normal.xyz, 0.0f), gInvTrWorldView);
	output.worldViewPos = mul(float4(input.pos.xyz, 1.0f), gWorldView);
	
	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float4 ambient = ambientSpecularPowType.x * float4(lightColor.xyz, 1.0f);
	float4 objColor = DiffuseMap.SampleLevel(Sampler, input.tex.xy, 0);

	float4 norm = normalize(input.normal);
	float diff = max(dot(norm, lightPos), 0.0f);
	float4 diffuse = diff * float4(lightColor.xyz, 1.0f);

	float4 reflectDir = reflect(-lightPos, norm);
	float3 viewDir = - normalize(input.worldViewPos.xyz);
	float spec = pow(max(dot(viewDir, reflectDir.xyz), 0.0f), ambientSpecularPowType.z);
	float4 specular = ambientSpecularPowType.y * spec * float4(lightColor.xyz, 1.0f);

	float4 result = (ambient + diffuse + specular) * objColor;
	
	return float4(result.xyz, 1.0f);
}