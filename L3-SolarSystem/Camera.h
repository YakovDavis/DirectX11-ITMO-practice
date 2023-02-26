#pragma once

#include "SimpleMath.h"

class Camera
{
protected:
    DirectX::SimpleMath::Matrix ViewProj;
public:
    Camera();
    virtual ~Camera();
    DirectX::SimpleMath::Matrix GetMatrix();
    void UpdateMatrix();
};
