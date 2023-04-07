#pragma pack_matrix(row_major)

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 4
#endif

struct PS_IN
{
	float4 pos : SV_POSITION;
 	float2 tex : TEXCOORD;
};

cbuffer cbPerScene : register(b0)
{
	float4 lightPos;
	float4 lightColor;
	float4 ambientSpecularPowType;
	float4x4 gT;
};

cbuffer cbCascade : register(b1)
{
	float4x4 gViewProj[CASCADE_COUNT + 1];
	float4 gDistances;
};

Texture2D<float4> DiffuseTex : register(t0);
Texture2D<float3> ViewPositions : register(t1);
Texture2D<float3> Normals : register(t2);
Texture2DArray CascadeShadowMap : register(t3);
SamplerComparisonState DepthSampler : register(s0);

PS_IN VSMain(uint id: SV_VertexID)
{
	PS_IN output = (PS_IN)0;

	output.tex = float2(id & 1, (id & 2) >> 1);
	output.pos = float4(output.tex * float2(2, -2) + float2(-1, 1), 0, 1);
	
	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float3 norm = normalize(Normals.Load(int3(input.pos.xy, 0)));
	
	//float shadow = ShadowCalculation(input.worldPos, input.viewPos, dot(norm, lightPos));
	
	float4 ambient = ambientSpecularPowType.x * float4(lightColor.xyz, 1.0f);
	float4 objColor = float4(DiffuseTex.Load(int3(input.pos.xy, 0)).xyz, 1.0f);

	float diff = max(dot(norm, lightPos.xyz), 0.0f);
	float4 diffuse = diff * float4(lightColor.xyz, 1.0f);

	float3 reflectDir = reflect(-lightPos.xyz, norm);
	float3 viewDir = normalize(-ViewPositions.Load(int3(input.pos.xy, 0)).xyz);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), ambientSpecularPowType.z);
	float4 specular = objColor.w * spec * float4(lightColor.xyz, 1.0f);

	float4 result = (ambient + (1.0f/* - shadow*/) * (diffuse + specular)) * objColor;

	//return float4(viewDir.xyz*0.5 + 0.5, 1.0f);
	
	return float4(result.xyz, 1.0f);
}
