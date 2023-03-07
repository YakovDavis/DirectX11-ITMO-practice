#pragma once

#include "BaseRenderComponent.h"

class TriangleComponent : public BaseRenderComponent
{
protected:
	CD3D11_RASTERIZER_DESC CreateRasterizerStateDesc() override;
public:
	explicit TriangleComponent(Game* g);
	void SetPosition(DirectX::SimpleMath::Vector3 p) override { position = p; }
	void SetRotation(DirectX::SimpleMath::Quaternion q) override { rotation = q; }
	void SetScale(DirectX::SimpleMath::Vector3 s) override { scale = s; }
};
