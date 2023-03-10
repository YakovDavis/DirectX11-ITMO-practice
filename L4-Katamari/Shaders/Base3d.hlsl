#pragma pack_matrix(row_major)

struct VS_IN
{
	float4 pos : POSITION0;
	float4 tex : TEXCOORD0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
 	float4 tex : TEXCOORD;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0);

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;
	
#ifdef VERTEX_PASS_THROUGH
	output.pos = float4(input.pos.xyz, 1.0f);
#else
	output.pos = mul(float4(input.pos.xyz, 1.0f), gWorldViewProj);
#endif
	output.tex = input.tex;
	
	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
#ifdef TREAT_TEX_AS_COL
	return input.tex;
#endif
	float4 color = DiffuseMap.SampleLevel(Sampler, input.tex.xy, 0);
	return color;
}