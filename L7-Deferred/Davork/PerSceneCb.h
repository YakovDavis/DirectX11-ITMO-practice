#pragma once
#include "SimpleMath.h"

struct PerSceneCb
{
    DirectX::SimpleMath::Vector4 DLightDir;
    DirectX::SimpleMath::Vector4 DLightColor;
    DirectX::SimpleMath::Vector4 AmbientSpecularPowType; // a - ambient light strength, s - specularity, p - falloff power
        // todo: t - 0: directional, 1: point, 2: spot
    DirectX::SimpleMath::Matrix T;
    DirectX::SimpleMath::Matrix ViewMatrix;
    DirectX::SimpleMath::Vector4 PLightPos[4];
    DirectX::SimpleMath::Vector4 PLightColor[4];
};
