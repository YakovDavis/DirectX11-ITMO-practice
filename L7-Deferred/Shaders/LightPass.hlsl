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
	float4x4 gView;
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

float ShadowCalculation(float4 posWorldSpace, float4 posViewSpace, float dotN)
{
	float depthValue = abs(posViewSpace.z);

	int layer = -1;
	for (int i = 0; i < CASCADE_COUNT; ++i)
	{
		if (depthValue < gDistances[i])
		{
			layer = i;
			break;
		}
	}
	if (layer == -1)
	{
		layer = CASCADE_COUNT;
	}

	float4 posLightSpace = mul(float4(posWorldSpace.xyz, 1.0), gViewProj[layer]);
	float3 projCoords = posLightSpace.xyz / posLightSpace.w;

	projCoords = (mul(float4(projCoords, 1.0f), gT)).xyz;
	float currentDepth = projCoords.z;

	if (currentDepth > 1.0f)
	{
		return 0.0f;
	}

	float bias = max(0.01f * (1.0f - dotN), 0.005f);
	const float biasModifier = 0.5f;
	if (layer == CASCADE_COUNT)
	{
		bias *= 1 / (1000.0 * biasModifier);
	}
	else
	{
		bias *= 1 / (gDistances[layer] * biasModifier);
	}

	// PCF
	float shadow = 0.0f;
	float2 texelSize = 1.0f / 2048.0f;
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			shadow += CascadeShadowMap.SampleCmp(DepthSampler, float3(projCoords.xy + float2(x, y) * texelSize, layer), currentDepth - bias);
		}
	}
	shadow /= 9.0f;

	//float shadow = CascadeShadowMap.SampleCmp(DepthSampler, float3(projCoords.xy, layer), currentDepth);

	return shadow;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float3 norm = normalize(Normals.Load(int3(input.pos.xy, 0)));
	
	float4 ambient = ambientSpecularPowType.x * float4(lightColor.xyz, 1.0f);
	float4 objColor = float4(DiffuseTex.Load(int3(input.pos.xy, 0)).xyz, 1.0f);

	float diff = max(dot(norm, lightPos.xyz), 0.0f);
	float4 diffuse = diff * float4(lightColor.xyz, 1.0f);

	float3 reflectDir = reflect(-lightPos.xyz, norm);
	float4 worldPos = float4(WorldPositions.Load(int3(input.pos.xy, 0)).xyz, 1.0f);
	float3 viewDir = normalize(-mul(worldPos, gView).xyz);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), ambientSpecularPowType.z);
	float4 specular = objColor.w * spec * float4(lightColor.xyz, 1.0f);

	float shadow = ShadowCalculation(worldPos, mul(worldPos, gView), dot(norm, lightPos));

	float4 result = (ambient + (1.0f - shadow) * (diffuse + specular)) * objColor;
	
	//return float4(viewDir.xyz*0.5 + 0.5, 1.0f);
	
	return float4(result.xyz, 1.0f);
}
