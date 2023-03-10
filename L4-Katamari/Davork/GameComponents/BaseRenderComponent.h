#pragma once

#include <vector>
#include "../GameComponent.h"
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "../Vertex.h"

class Game;

class BaseRenderComponent : public GameComponent
{
public:
    struct CBDataPerObject
    {
        DirectX::SimpleMath::Matrix worldViewProj;
        DirectX::SimpleMath::Matrix invTrWorld;
    };
    struct CBDataPerScene
    {
        DirectX::SimpleMath::Vector4 lightPos;
        DirectX::SimpleMath::Vector4 lightColorAmbStr;
        DirectX::SimpleMath::Vector4 viewDirSpecStr;
    };
protected:
    ID3D11InputLayout* layout;
    //ID3D11PixelShader* pixelShader;
    //ID3DBlob* pixelShaderByteCode;
    ID3D11RasterizerState* rastState;
    //ID3D11VertexShader* vertexShader;
    //ID3DBlob* vertexShaderByteCode;
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    ID3D11Buffer** constBuffers;
    UINT strides[1];
    UINT offsets[1];
    std::vector<Vertex> points{};
    std::vector<UINT> indices{};
    bool passThroughVS;
    bool colorModePS;
    D3D_PRIMITIVE_TOPOLOGY topologyType;
    virtual CD3D11_RASTERIZER_DESC CreateRasterizerStateDesc();
    const wchar_t* textureFileName;
    //ID3D11Resource* diffuseTextureBuffer;
    //ID3D11ShaderResourceView* diffuseTextureView;
    ID3D11SamplerState* samplerState;
public:
    BaseRenderComponent(Game* game);
    void DestroyResources() override;
    void Draw() override;
    void Initialize() override;
    void Reload() override;
    void Update() override;
};
