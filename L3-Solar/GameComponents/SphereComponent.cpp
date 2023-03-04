#include "SphereComponent.h"
#include "../Davork/Game.h"

using namespace DirectX;
using namespace SimpleMath;

SphereComponent::SphereComponent(Game* g, float radius, int sliceCount, int stackCount, Vector4 col1, Vector4 col2)
	: GameComponent(g), rotation(Quaternion::Identity), position(Vector3::Zero)
{
	Point topPoint( {Vector4(0.0f, radius, 0.0f, 1.0f), col1} );
	Vector4::Lerp(col1, col2, 0.5f, topPoint.col);
	Point bottomPoint( {Vector4(0.0f, -radius, 0.0f, 1.0f), col1} );
	Vector4::Lerp(col1, col2, 0.5f, bottomPoint.col);

	points.push_back(topPoint);

	const float phiStep   = XM_PI / static_cast<float>(stackCount);
	const float thetaStep = XM_2PI / static_cast<float>(sliceCount);
	
	for(int i = 1; i <= stackCount-1; ++i)
	{
		const float phi = static_cast<float>(i) * phiStep;
		
        for(int j = 0; j <= sliceCount; ++j)
		{
			const float theta = static_cast<float>(j) * thetaStep;
			Point p;
			p.pos.x = radius * sinf(phi)*cosf(theta);
			p.pos.y = radius * cosf(phi);
			p.pos.z = radius * sinf(phi)*sinf(theta);
        	p.pos.w = 1.0f;
        	Vector4::Lerp(col1, col2, (sinf(2.0f * phi) * sinf(theta) + 1.0f) / 2.0f, p.col);
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

void SphereComponent::Initialize()
{
	ID3DBlob* errorVertexCode = nullptr;
	auto res = D3DCompileFromFile(L"./Shaders/Base3dCol.hlsl",
		nullptr /*macros*/,
		nullptr /*include*/,
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
			MessageBox(game->Display->hWnd, L"Base3dCol.hlsl", L"Missing Shader File", MB_OK);
		}

		return;
	}

	ID3DBlob* errorPixelCode;
	res = D3DCompileFromFile(L"./Shaders/Base3dCol.hlsl",
		nullptr /*macros*/,
		nullptr /*include*/,
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
		nullptr, &pixelShader);

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
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0}
	};

	game->Device->CreateInputLayout(
		inputElements,
		2,
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		&layout);

	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(Vector4) * std::size(points) * 2;

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
	indexBufDesc.ByteWidth = sizeof(int) * std::size(indices);

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	game->Device->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);

	strides[0] = 32;
	offsets[0] = 0;

	D3D11_BUFFER_DESC constBufDesc = {};
	constBufDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufDesc.CPUAccessFlags = 0;
	constBufDesc.MiscFlags = 0;
	constBufDesc.StructureByteStride = 0;
	constBufDesc.ByteWidth = sizeof(SimpleMath::Matrix);

	game->Device->CreateBuffer(&constBufDesc, nullptr, &constBuffer);

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	res = game->Device->CreateRasterizerState(&rastDesc, &rastState);
}

void SphereComponent::Update()
{
	const Matrix world = Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(position);
	Matrix worldViewProj = world * game->Camera->GetMatrix();
	worldViewProj = worldViewProj.Transpose();
	game->Context->UpdateSubresource(constBuffer, 0, nullptr, &worldViewProj, 0, 0);
}

void SphereComponent::Reload()
{
}

void SphereComponent::Draw()
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
	game->Context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	game->Context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	game->Context->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
	game->Context->VSSetShader(vertexShader, nullptr, 0);
	game->Context->VSSetConstantBuffers(0, 1, &constBuffer);
	game->Context->PSSetShader(pixelShader, nullptr, 0);

	game->Context->DrawIndexed(indices.size(), 0, 0);
}

void SphereComponent::DestroyResources()
{
	layout->Release();
	pixelShader->Release();
	pixelShaderByteCode->Release();
	rastState->Release();
	vertexShader->Release();
	vertexShaderByteCode->Release();
	vertexBuffer->Release();
	indexBuffer->Release();
}