#include "QuadComponent.h"
#include "../Game.h"

using namespace DirectX;
using namespace SimpleMath;

CD3D11_RASTERIZER_DESC QuadComponent::CreateRasterizerStateDesc()
{
	CD3D11_RASTERIZER_DESC desc = BaseRenderComponent::CreateRasterizerStateDesc();
	desc.CullMode = D3D11_CULL_NONE;
	return desc;
}

QuadComponent::QuadComponent(Game* g, const wchar_t* diffuseTextureName) : BaseRenderComponent(g)
{
	textureFileName = diffuseTextureName;
	
	points.push_back( { Vector4(0.5f, 0.5f, 0.0f, 1.0f),	Vector4(16.0f, 16.0f, 0.0f, 0.0f) } );
	points.push_back( { Vector4(-0.5f, -0.5f, 0.0f, 1.0f),Vector4(0.0f, 0.0f, 0.0f, 0.0f) } );
	points.push_back( { Vector4(0.5f, -0.5f, 0.0f, 1.0f), Vector4(0.0f, 16.0f, 0.0f, 0.0f) } );
	points.push_back( { Vector4(-0.5f, 0.5f, 0.0f, 1.0f), Vector4(16.0f, 0.0f, 0.0f, 0.0f) } );

	indices = { 0,1,2, 1,0,3 };
}
