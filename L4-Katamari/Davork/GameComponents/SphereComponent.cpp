﻿#include "SphereComponent.h"
#include "../Game.h"

using namespace DirectX;
using namespace SimpleMath;

SphereComponent::SphereComponent(Game* g, float radius, int sliceCount, int stackCount, const wchar_t* diffuseTextureName)
	: BaseRenderComponent(g)
{
	textureFileName = diffuseTextureName;
	
	Vertex topPoint( {Vector4(0.0f, radius, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 0.0f) } );
	Vertex bottomPoint( {Vector4(0.0f, -radius, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 0.0f) } );

	points.push_back(topPoint);

	const float phiStep   = XM_PI / static_cast<float>(stackCount);
	const float thetaStep = XM_2PI / static_cast<float>(sliceCount);
	
	for(int i = 1; i <= stackCount-1; ++i)
	{
		const float phi = static_cast<float>(i) * phiStep;
		
        for(int j = 0; j <= sliceCount; ++j)
		{
			const float theta = static_cast<float>(j) * thetaStep;
			Vertex p;
			p.pos.x = radius * sinf(phi)*cosf(theta);
			p.pos.y = radius * cosf(phi);
			p.pos.z = radius * sinf(phi)*sinf(theta);
        	p.pos.w = 1.0f;
        	p.tex.x = theta / XM_2PI;
        	p.tex.y = phi / XM_PI;
			points.push_back(p);
		}
	}
	
	points.push_back(bottomPoint);

    for(int i = 1; i <= sliceCount; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}
	
    int baseIndex = 1;
	const int ringVertexCount = sliceCount + 1;
	for(int i = 0; i < stackCount - 2; ++i)
	{
		for(int j = 0; j < sliceCount; ++j)
		{
			indices.push_back(baseIndex + i*ringVertexCount + j);
			indices.push_back(baseIndex + i*ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i+1)*ringVertexCount + j);

			indices.push_back(baseIndex + (i+1)*ringVertexCount + j);
			indices.push_back(baseIndex + i*ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i+1)*ringVertexCount + j + 1);
		}
	}
	
	const int southPoleIndex = points.size() - 1;
	
	baseIndex = southPoleIndex - ringVertexCount;
	
	for(int i = 0; i < sliceCount; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}
}
