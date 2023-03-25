#pragma once
#include "SimpleMath.h"
#include <vector>
#include "PerSceneCb.h"

class DirectionalLight
{
    DirectX::SimpleMath::Vector4 lightDirection_;
    DirectX::SimpleMath::Vector4 lightColor_;
public:
    DirectionalLight();
    DirectX::SimpleMath::Vector4 GetDirection() const { return lightDirection_; }
    void SetDirection(const DirectX::SimpleMath::Vector4 dir) { lightDirection_ = dir; }
    DirectX::SimpleMath::Vector4 GetColor() const { return lightColor_; }
    void SetColor(const DirectX::SimpleMath::Vector4 col) { lightColor_ = col; }
    void FillCb(PerSceneCb& cb) const;
    DirectX::SimpleMath::Matrix GetLightViewProj(std::vector<DirectX::SimpleMath::Vector4> corners) const;
};
