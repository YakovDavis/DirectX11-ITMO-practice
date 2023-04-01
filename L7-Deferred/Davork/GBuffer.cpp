#include "GBuffer.h"

#include "Game.h"

void GBuffer::CreateDepthStencilBuffer()
{
    D3D11_TEXTURE2D_DESC depthBufferDesc = {};
    depthBufferDesc.Width = game_->GetDisplay()->ClientWidth;
    depthBufferDesc.Height = game_->GetDisplay()->ClientHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0; 
    depthBufferDesc.MiscFlags = 0;

    auto res = game_->GetDevice()->CreateTexture2D(&depthBufferDesc, nullptr, depthBuffer_.GetAddressOf());

    if (FAILED(res))
    {
        OutputDebugString(TEXT("Fatal error: Failed to create depth stencil buffer!\n"));
    }
	
    res = game_->GetDevice()->CreateDepthStencilView(depthBuffer_.Get(), nullptr, depthDsv_.GetAddressOf());

    if (FAILED(res))
    {
        OutputDebugString(TEXT("Fatal error: Failed to create depth stencil view!\n"));
    }
}

GBuffer::GBuffer(Game* g) : game_(g)
{
}

void GBuffer::Initialize()
{
    const int width = game_->GetDisplay()->ClientWidth;
    const int height = game_->GetDisplay()->ClientHeight;

    D3D11_TEXTURE2D_DESC baseBuffDesc = {};
    baseBuffDesc.Width = width;
    baseBuffDesc.Height = height;
    baseBuffDesc.ArraySize = 1;
    baseBuffDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    baseBuffDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    baseBuffDesc.MipLevels = 1;
    baseBuffDesc.SampleDesc.Count = 1;
    baseBuffDesc.SampleDesc.Quality = 0;
    baseBuffDesc.Usage = D3D11_USAGE_DEFAULT;
    baseBuffDesc.CPUAccessFlags = 0;
    baseBuffDesc.MiscFlags = 0;

    D3D11_SHADER_RESOURCE_VIEW_DESC baseSrvDesc = {};
    baseSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

    D3D11_RENDER_TARGET_VIEW_DESC baseRtvDesc = {};

    D3D11_TEXTURE2D_DESC albedoBuffDesc = baseBuffDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC albedoSrvDesc = baseSrvDesc;
    D3D11_RENDER_TARGET_VIEW_DESC albedoRtvDesc = baseRtvDesc;
    auto res = game_->GetDevice()->CreateTexture2D(&albedoBuffDesc, nullptr, albedoBuffer_.GetAddressOf());
    res = game_->GetDevice()->CreateShaderResourceView(albedoBuffer_.Get(), &albedoSrvDesc, albedoSrv_.GetAddressOf());
    res = game_->GetDevice()->CreateRenderTargetView(albedoBuffer_.Get(), &albedoRtvDesc, albedoRtv_.GetAddressOf());

    D3D11_TEXTURE2D_DESC normalBuffDesc = baseBuffDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC normalSrvDesc = baseSrvDesc;
    D3D11_RENDER_TARGET_VIEW_DESC normalRtvDesc = baseRtvDesc;
    res = game_->GetDevice()->CreateTexture2D(&normalBuffDesc, nullptr, normalBuffer_.GetAddressOf());
    res = game_->GetDevice()->CreateShaderResourceView(normalBuffer_.Get(), &albedoSrvDesc, normalSrv_.GetAddressOf());
    res = game_->GetDevice()->CreateRenderTargetView(normalBuffer_.Get(), &normalRtvDesc, normalRtv_.GetAddressOf());

    D3D11_TEXTURE2D_DESC specularBuffDesc = baseBuffDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC specularSrvDesc = baseSrvDesc;
    D3D11_RENDER_TARGET_VIEW_DESC specularRtvDesc = baseRtvDesc;
    res = game_->GetDevice()->CreateTexture2D(&specularBuffDesc, nullptr, specularBuffer_.GetAddressOf());
    res = game_->GetDevice()->CreateShaderResourceView(specularBuffer_.Get(), &specularSrvDesc, specularSrv_.GetAddressOf());
    res = game_->GetDevice()->CreateRenderTargetView(specularBuffer_.Get(), &specularRtvDesc, specularRtv_.GetAddressOf());

    D3D11_TEXTURE2D_DESC positionBuffDesc = baseBuffDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC positionSrvDesc = baseSrvDesc;
    D3D11_RENDER_TARGET_VIEW_DESC positionRtvDesc = baseRtvDesc;
    res = game_->GetDevice()->CreateTexture2D(&positionBuffDesc, nullptr, positionBuffer_.GetAddressOf());
    res = game_->GetDevice()->CreateShaderResourceView(positionBuffer_.Get(), &positionSrvDesc, positionSrv_.GetAddressOf());
    res = game_->GetDevice()->CreateRenderTargetView(positionBuffer_.Get(), &positionRtvDesc, positionRtv_.GetAddressOf());
    
    CreateDepthStencilBuffer();
}
