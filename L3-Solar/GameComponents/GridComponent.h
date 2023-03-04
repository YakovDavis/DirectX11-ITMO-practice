#pragma once

#pragma once
#include <vector>
#include "../Davork/GameComponent.h"
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "../Davork/SimpleMath.h"

class Game;

class GridComponent : public GameComponent
{
public:
    struct Point
    {
        DirectX::SimpleMath::Vector4 pos;
        DirectX::SimpleMath::Vector4 col;
    };
protected:
    ID3D11InputLayout* layout;
    ID3D11PixelShader* pixelShader;
    ID3DBlob* pixelShaderByteCode;
    ID3D11RasterizerState* rastState;
    ID3D11VertexShader* vertexShader;
    ID3DBlob* vertexShaderByteCode;
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* constBuffer;
    UINT strides[1];
    UINT offsets[1];
    std::vector<Point> points;
    std::vector<int> indices;
public:
    GridComponent(Game* game, float cellSize, int lineCount);
    void DestroyResources() override;
    void Draw() override;
    void Initialize() override;
    void Update() override;
    void Reload() override;
};
