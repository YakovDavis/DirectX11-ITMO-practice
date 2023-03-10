#include "BaseRenderComponent.h"
#include "../Game.h"
#include "../DDSTextureLoader.h"
#include "../ResourceFactory.h"

#pragma warning(disable : 4267)

//#define CBUFFER_MAPPING

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

BaseRenderComponent::BaseRenderComponent(Game* g)
	: GameComponent(g), layout(nullptr),/* pixelShader(nullptr), pixelShaderByteCode(nullptr), rastState(nullptr),
	vertexShader(nullptr), vertexShaderByteCode(nullptr),*/ vertexBuffer(nullptr), indexBuffer(nullptr),
	constBufferPerObject(nullptr), strides{}, offsets{}, passThroughVS(false), colorModePS(false),
	topologyType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST), textureFileName(L"Textures/wood.dds")
{
}

void BaseRenderComponent::Initialize()
{
	D3D_SHADER_MACRO ShaderMacros[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	int macrosCount = 0;
	if (passThroughVS)
	{
		ShaderMacros[macrosCount++] = { "VERTEX_PASS_THROUGH", "1" };
	}

	if (colorModePS)
	{
		ShaderMacros[macrosCount++] = { "TREAT_TEX_AS_COL", "1" };
	}
	
	/*ID3DBlob* errorVertexCode = nullptr;
	auto res = D3DCompileFromFile(L"./Shaders/Base3d.hlsl",
		ShaderMacros,
		nullptr,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexShaderByteCode,
		&errorVertexCode);

	if (FAILED(res)) {
		// If the shader failed to compile it should have written something to the error message.
		if (errorVertexCode) {
			const char* compileErrors = static_cast<char*>(errorVertexCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(game->Display->hWnd, L"Base3d.hlsl", L"Missing Shader File", MB_OK);
		}

		return;
	}

	ID3DBlob* errorPixelCode;
	res = D3DCompileFromFile(L"./Shaders/Base3d.hlsl",
		ShaderMacros,
		nullptr,
		"PSMain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelShaderByteCode,
		&errorPixelCode);

	game->Device->CreateVertexShader(
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		nullptr, &vertexShader);

	game->Device->CreatePixelShader(
		pixelShaderByteCode->GetBufferPointer(),
		pixelShaderByteCode->GetBufferSize(),
		nullptr, &pixelShader);*/

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
			0}
	};

	/*game->Device->CreateInputLayout(
		inputElements,
		2,
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		&layout);*/
	game->Device->CreateInputLayout(
		inputElements,
		2,
		ResourceFactory::GetVertexShaderBC("base")->GetBufferPointer(),
		ResourceFactory::GetVertexShaderBC("base")->GetBufferSize(),
		&layout);

	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(Vertex) * std::size(points);

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = points.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	game->Device->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);
	
	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(UINT) * std::size(indices);

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	game->Device->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);

	strides[0] = 32;
	offsets[0] = 0;

	D3D11_BUFFER_DESC constBufPerObjDesc = {};
	constBufPerObjDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
#ifdef CBUFFER_MAPPING
	constBufPerObjDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufPerObjDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
#else
	constBufPerObjDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufPerObjDesc.CPUAccessFlags = 0;
#endif
	constBufPerObjDesc.MiscFlags = 0;
	constBufPerObjDesc.StructureByteStride = 0;
	constBufPerObjDesc.ByteWidth = sizeof(SimpleMath::Matrix);

	game->Device->CreateBuffer(&constBufPerObjDesc, nullptr, &constBufferPerObject);

	/*auto res = CreateDDSTextureFromFile(game->Device.Get(), textureFileName, &diffuseTextureBuffer, &diffuseTextureView);
	game->Context->GenerateMips(diffuseTextureView);*/

	D3D11_SAMPLER_DESC samplerStateDesc = {};
	samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerStateDesc.MaxLOD = INT_MAX;
	
	auto res = game->Device->CreateSamplerState(&samplerStateDesc, &samplerState);

	const CD3D11_RASTERIZER_DESC rastDesc = CreateRasterizerStateDesc();

	res = game->Device->CreateRasterizerState(&rastDesc, &rastState);
}

void BaseRenderComponent::Reload()
{
}

void BaseRenderComponent::Draw()
{
	game->Context->RSSetState(rastState);

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(game->Display->ClientWidth);
	viewport.Height = static_cast<float>(game->Display->ClientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	game->Context->RSSetViewports(1, &viewport);

	game->Context->IASetInputLayout(layout);
	game->Context->IASetPrimitiveTopology(topologyType);
	game->Context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	game->Context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
	game->Context->VSSetShader(ResourceFactory::GetVertexShader("base"), nullptr, 0);
	game->Context->VSSetConstantBuffers(0, 1, &constBufferPerObject);
	game->Context->PSSetShader(ResourceFactory::GetPixelShader("base"), nullptr, 0);
	ID3D11ShaderResourceView* test = ResourceFactory::GetTextureView(textureFileName);
	game->Context->PSSetShaderResources(0, 1, &test);
	game->Context->PSSetSamplers(0, 1, &samplerState);

	game->Context->DrawIndexed(indices.size(), 0, 0);
}

void BaseRenderComponent::DestroyResources()
{
	layout->Release();
	//pixelShader->Release();
	//pixelShaderByteCode->Release();
	rastState->Release();
	//vertexShader->Release();
	//vertexShaderByteCode->Release();
	vertexBuffer->Release();
	indexBuffer->Release();
}

void BaseRenderComponent::Update()
{
	rotation.Normalize();
	const Matrix world = Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(position);
	const Matrix worldViewProj = (world * game->Camera->GetMatrix()).Transpose();

#ifdef CBUFFER_MAPPING
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	game->Context->Map(constBufferPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &worldViewProj, sizeof(worldViewProj));
	game->Context->Unmap(constBufferPerObject, 0);
#else
	game->Context->UpdateSubresource(constBufferPerObject, 0, nullptr, &worldViewProj, 0 ,0);
#endif
}