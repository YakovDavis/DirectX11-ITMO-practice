#include "TriangleComponent.h"
#include "../Game.h"

using namespace DirectX;
using namespace SimpleMath;

CD3D11_RASTERIZER_DESC TriangleComponent::CreateRasterizerStateDesc()
{
	CD3D11_RASTERIZER_DESC desc = BaseRenderComponent::CreateRasterizerStateDesc();
	desc.CullMode = D3D11_CULL_NONE;
	return desc;
}

TriangleComponent::TriangleComponent(Game* g) : BaseRenderComponent(g)
{
	points.push_back( { Vector4(0.5f, 0.5f, 0.0f, 1.0f),	Vector4(1.0f, 1.0f, 0.0f, 0.0f) } );
	points.push_back( { Vector4(-0.5f, -0.5f, 0.0f, 1.0f),Vector4(0.0f, 0.0f, 0.0f, 0.0f) } );
	points.push_back( { Vector4(0.5f, -0.5f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 0.0f) } );

	indices = { 0,1,2 };
}
