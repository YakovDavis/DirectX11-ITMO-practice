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
Texture2D<float3> WorldPositions : register(t1);
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
	return DiffuseTex.Load(int3(input.pos.xy, 0));
}
