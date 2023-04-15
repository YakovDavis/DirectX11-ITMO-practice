#include "LightVolumeComponent.h"
#include "../ResourceFactory.h"
#include "../Game.h"

CD3D11_RASTERIZER_DESC LightVolumeComponent::CreateRasterizerStateDesc()
{
    auto rs = SphereComponent::CreateRasterizerStateDesc();
    rs.CullMode = D3D11_CULL_FRONT;
    return rs;
}

LightVolumeComponent::LightVolumeComponent(Game* g) : SphereComponent(g, 1.0f, 16, 16, L"Textures/duck.dds")
{
    isShadowCasting_ = false;
}

void LightVolumeComponent::SetSize(float size)
{
    SphereComponent::SetScale(DirectX::SimpleMath::Vector3(size, size, size));
}

void LightVolumeComponent::Draw()
{
    game->GetContext()->RSSetState(rastState_.Get());
    
    game->GetContext()->IASetInputLayout(layout_.Get());
    game->GetContext()->IASetPrimitiveTopology(topologyType_);
    game->GetContext()->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);
    game->GetContext()->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), strides_, offsets_);
    game->GetContext()->VSSetShader(ResourceFactory::GetVertexShader("lightpassvolume"), nullptr, 0);
    game->GetContext()->VSSetConstantBuffers(0, 1, constBuffers_[0].GetAddressOf());
    game->GetContext()->PSSetShader(ResourceFactory::GetPixelShader("lightpassvolume"), nullptr, 0);

    game->GetContext()->DrawIndexed(indices_.size(), 0, 0);
}
