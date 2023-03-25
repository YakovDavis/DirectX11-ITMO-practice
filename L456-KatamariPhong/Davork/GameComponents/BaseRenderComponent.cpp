#include "BaseRenderComponent.h"
#include "../Game.h"
#include "../DDSTextureLoader.h"
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
	  constBuffers_(new ID3D11Buffer*[2]), strides_{}, offsets_{}, passThroughVs_(false), colorModePs_(false),
	  topologyType_(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST), textureFileName_(L"Textures/wood.dds"), samplerState_(nullptr)
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

	D3D11_BUFFER_DESC constBufPerObjDesc;
	constBufPerObjDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufPerObjDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufPerObjDesc.CPUAccessFlags = 0;
	constBufPerObjDesc.MiscFlags = 0;
	constBufPerObjDesc.StructureByteStride = 0;
	constBufPerObjDesc.ByteWidth = sizeof(CbDataPerObject);

	game->GetDevice()->CreateBuffer(&constBufPerObjDesc, nullptr, &constBuffers_[0]);

	D3D11_BUFFER_DESC constBufPerSceneDesc;
	constBufPerSceneDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufPerSceneDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufPerSceneDesc.CPUAccessFlags = 0;
	constBufPerSceneDesc.MiscFlags = 0;
	constBufPerSceneDesc.StructureByteStride = 0;
	constBufPerSceneDesc.ByteWidth = sizeof(CbDataPerScene);

	game->GetDevice()->CreateBuffer(&constBufPerSceneDesc, nullptr, &constBuffers_[1]);

	D3D11_SAMPLER_DESC samplerStateDesc = {};
	samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerStateDesc.MinLOD = 0.0f;
	samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	auto res = game->GetDevice()->CreateSamplerState(&samplerStateDesc, samplerState_.GetAddressOf());

	const CD3D11_RASTERIZER_DESC rastDesc = CreateRasterizerStateDesc();

	res = game->GetDevice()->CreateRasterizerState(&rastDesc, rastState_.GetAddressOf());
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
	game->GetContext()->VSSetShader(ResourceFactory::GetVertexShader("base"), nullptr, 0);
	game->GetContext()->VSSetConstantBuffers(0, 2, constBuffers_);
	game->GetContext()->PSSetConstantBuffers(0, 2, constBuffers_);
	game->GetContext()->PSSetShader(ResourceFactory::GetPixelShader("base"), nullptr, 0);
	const auto test = ResourceFactory::GetTextureView(textureFileName_);
	game->GetContext()->PSSetShaderResources(0, 1, &test);
	game->GetContext()->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

	game->GetContext()->DrawIndexed(indices_.size(), 0, 0);
}

void BaseRenderComponent::DestroyResources()
{
	constBuffers_[0]->Release();
	constBuffers_[1]->Release();
	delete[] constBuffers_;
}

void BaseRenderComponent::Update()
{
	rotation.Normalize();
	const Matrix world = Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(position);
	
	CbDataPerObject objData = {};
	objData.WorldViewProj = world * game->GetCamera()->GetMatrix();
	objData.InvTrWorld = (Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation)).Invert().Transpose();
	
	CbDataPerScene sceneData = {};
	sceneData.LightPos = Vector4(1.0f, 1.0f, 1.0f, 0.0f);
	sceneData.LightColorAmbStr = Vector4(1.0f, 1.0f, 1.0f, 0.4f);
	sceneData.ViewDirSpecStr = Vector4(game->GetCamera()->Position.x - game->GetCamera()->Target.x, game->GetCamera()->Position.y - game->GetCamera()->Target.y,  game->GetCamera()->Position.z - game->GetCamera()->Target.z, 0.0f);
	sceneData.ViewDirSpecStr.Normalize();
	sceneData.ViewDirSpecStr.w = 0.5f;
	sceneData.LightPos.Normalize();
	
	game->GetContext()->UpdateSubresource(constBuffers_[0], 0, nullptr, &objData, 0, 0);
	game->GetContext()->UpdateSubresource(constBuffers_[1], 0, nullptr, &sceneData, 0, 0);
}