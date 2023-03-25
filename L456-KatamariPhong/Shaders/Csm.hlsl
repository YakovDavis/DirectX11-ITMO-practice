#pragma pack_matrix(row_major)

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 4
#endif

struct VS_IN
{
	float4 pos : POSITION0;
	float4 tex : TEXCOORD0;
	float4 normal : NORMAL0;
};

struct GS_IN
{
	float4 pos : POSITION;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
	float4x4 gInvTrWorld;
};

GS_IN VSMain(VS_IN input)
{
	GS_IN output = (GS_IN)0;

	output.pos = mul(float4(input.pos.xyz, 1.0f), gWorldViewProj);

	return output;
}

cbuffer cbCascade : register(b0)
{
	float4x4 gViewProj[CASCADE_COUNT];
	float4 gDistances;
};

struct GS_OUT
{
	float4 pos : SV_POSITION;
 	uint arrInd : SV_RenderTargetArrayIndex;
};

[instance(4)]
[maxvertexcount(3)]
void GSMain(triangle GS_IN p[3], in uint id : SV_GSInstanceID, inout TriangleStream<GS_OUT> stream)
{
	[unroll]
	for (int i = 0; i < CASCADE_COUNT - 1; ++i)
	{
		GS_OUT gs = (GS_OUT)0;
		gs.pos = mul(float4(p[i].pos.xyz, 1.0f), gViewProj[id]);
		gs.arrInd = id;
		stream.Append(gs);
	}
}
