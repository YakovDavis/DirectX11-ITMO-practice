#include "BaseRenderComponent.h"
#include "../Game.h"
#include "../DDSTextureLoader.h"
#include "../PerSceneCb.h"
#include "../ResourceFactory.h"

#pragma warning(disable : 4267)

using namespace DirectX;
using namespace SimpleMath;

CD3D11_RASTERIZER_DESC BaseRenderComponent::CreateRasterizerStateDesc()
{
	CD3D11_RASTERIZER_DESC rastDesc = {};
	
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.FrontCounterClockwise = true;
	rastDesc.DepthClipEnable = true;
	
	return rastDesc;
}

BaseRenderComponent::BaseRenderComponent(Game* game)
	: GameComponent(game), layout_(nullptr), rastState_(nullptr), vertexBuffer_(nullptr), indexBuffer_(nullptr),
	  strides_{}, offsets_{}, passThroughVs_(false), colorModePs_(false),
	  topologyType_(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST), textureFileName_(L"Textures/wood.dds"), samplerState_(nullptr),
	  isShadowCasting_(true)
{
}

void BaseRenderComponent::Initialize()
{
	constexpr D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0}
	};
	
	game->GetDevice()->CreateInputLayout(
		inputElements,
		3,
		ResourceFactory::GetVertexShaderBC("base")->GetBufferPointer(),
		ResourceFactory::GetVertexShaderBC("base")->GetBufferSize(),
		layout_.GetAddressOf());

	D3D11_BUFFER_DESC vertexBufDesc;
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(Vertex) * std::size(points_);

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = points_.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	game->GetDevice()->CreateBuffer(&vertexBufDesc, &vertexData, vertexBuffer_.GetAddressOf());
	
	D3D11_BUFFER_DESC indexBufDesc;
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(UINT) * std::size(indices_);

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices_.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	game->GetDevice()->CreateBuffer(&indexBufDesc, &indexData, indexBuffer_.GetAddressOf());

	strides_[0] = sizeof(Vertex);
	offsets_[0] = 0;

	constBuffers_.push_back(nullptr);
	constBuffers_.push_back(nullptr);

	D3D11_BUFFER_DESC constBufPerObjDesc;
	constBufPerObjDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufPerObjDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufPerObjDesc.CPUAccessFlags = 0;
	constBufPerObjDesc.MiscFlags = 0;
	constBufPerObjDesc.StructureByteStride = 0;
	constBufPerObjDesc.ByteWidth = sizeof(CbDataPerObject);

	game->GetDevice()->CreateBuffer(&constBufPerObjDesc, nullptr, constBuffers_[0].GetAddressOf());

	D3D11_BUFFER_DESC constBufCascadeDesc;
	constBufCascadeDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufCascadeDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufCascadeDesc.CPUAccessFlags = 0;
	constBufCascadeDesc.MiscFlags = 0;
	constBufCascadeDesc.StructureByteStride = 0;
	constBufCascadeDesc.ByteWidth = sizeof(Matrix) * 5 + sizeof(Vector4);

	game->GetDevice()->CreateBuffer(&constBufCascadeDesc, nullptr, constBuffers_[1].GetAddressOf());

	D3D11_SAMPLER_DESC samplerStateDesc = {};
	samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerStateDesc.MinLOD = 0.0f;
	samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	auto res = game->GetDevice()->CreateSamplerState(&samplerStateDesc, samplerState_.GetAddressOf());

	D3D11_SAMPLER_DESC depthSamplerStateDesc = {};
	depthSamplerStateDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	depthSamplerStateDesc.ComparisonFunc = D3D11_COMPARISON_GREATER_EQUAL;
	depthSamplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	depthSamplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	depthSamplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	depthSamplerStateDesc.BorderColor[0] = 1.0f;
	depthSamplerStateDesc.BorderColor[1] = 1.0f;
	depthSamplerStateDesc.BorderColor[2] = 1.0f;
	depthSamplerStateDesc.BorderColor[3] = 1.0f;

	res = game->GetDevice()->CreateSamplerState(&depthSamplerStateDesc, depthSamplerState_.GetAddressOf());

	const CD3D11_RASTERIZER_DESC rastDesc = CreateRasterizerStateDesc();

	res = game->GetDevice()->CreateRasterizerState(&rastDesc, rastState_.GetAddressOf());

	CD3D11_RASTERIZER_DESC shadowRastDesc = CreateRasterizerStateDesc();

	shadowRastDesc.CullMode = D3D11_CULL_FRONT;
	shadowRastDesc.DepthClipEnable = false;

	res = game->GetDevice()->CreateRasterizerState(&shadowRastDesc, shadowRastState_.GetAddressOf());
}

void BaseRenderComponent::Reload()
{
}

void BaseRenderComponent::Draw()
{
	game->GetContext()->RSSetState(rastState_.Get());

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(game->GetDisplay()->ClientWidth);
	viewport.Height = static_cast<float>(game->GetDisplay()->ClientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	game->GetContext()->RSSetViewports(1, &viewport);

	game->GetContext()->IASetInputLayout(layout_.Get());
	game->GetContext()->IASetPrimitiveTopology(topologyType_);
	game->GetContext()->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);
	game->GetContext()->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), strides_, offsets_);
	game->GetContext()->VSSetShader(ResourceFactory::GetVertexShader("gbuffer"), nullptr, 0);
	game->GetContext()->VSSetConstantBuffers(0, 1, constBuffers_[0].GetAddressOf());
	game->GetContext()->PSSetShader(ResourceFactory::GetPixelShader("gbuffer"), nullptr, 0);
	const auto texture = ResourceFactory::GetTextureView(textureFileName_);
	game->GetContext()->PSSetShaderResources(0, 1, &texture);
	game->GetContext()->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

	game->GetContext()->DrawIndexed(indices_.size(), 0, 0);
}

void BaseRenderComponent::DestroyResources()
{
}

void BaseRenderComponent::PrepareFrame()
{
	if (!isShadowCasting_)
		return;

	game->GetContext()->RSSetState(rastState_.Get());

	D3D11_VIEWPORT viewport;
	viewport.Width = 1024.0f;
	viewport.Height = 1024.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	game->GetContext()->RSSetViewports(1, &viewport);

	game->GetContext()->IASetInputLayout(layout_.Get());
	game->GetContext()->IASetPrimitiveTopology(topologyType_);
	game->GetContext()->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);
	game->GetContext()->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), strides_, offsets_);
	game->GetContext()->VSSetShader(ResourceFactory::GetVertexShader("csm"), nullptr, 0);
	game->GetContext()->VSSetConstantBuffers(0, 1, constBuffers_[0].GetAddressOf());
	game->GetContext()->PSSetShader(nullptr, nullptr, 0);
	game->GetContext()->GSSetShader(ResourceFactory::GetGeometryShader("csm"), nullptr, 0);
	game->GetContext()->GSSetConstantBuffers(0, 1, constBuffers_[1].GetAddressOf());

	game->GetContext()->DrawIndexed(indices_.size(), 0, 0);
}

void BaseRenderComponent::Update()
{
	rotation.Normalize();
	const Matrix world = Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(position);
	
	CbDataPerObject objData = {};
	objData.WorldViewProj = world * game->GetCamera()->GetViewProj();
	objData.World = world;
	objData.WorldView = world * game->GetCamera()->GetView();
	objData.InvTrWorldView = (world * game->GetCamera()->GetView()).Invert().Transpose();

	CbDataCascade cascadeData = {};
	auto tmp = game->GetDLight()->GetLightSpaceMatrices();
	for (int i = 0; i < 5; ++i)
	{
		cascadeData.ViewProj[i] = tmp[i];
	}
	cascadeData.Distance = game->GetDLight()->GetShadowCascadeDistances();

	game->GetContext()->UpdateSubresource(constBuffers_[0].Get(), 0, nullptr, &objData, 0, 0);
	game->GetContext()->UpdateSubresource(constBuffers_[1].Get(), 0, nullptr, &cascadeData, 0, 0);
}
