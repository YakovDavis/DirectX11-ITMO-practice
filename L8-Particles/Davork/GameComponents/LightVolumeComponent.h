#pragma once
#include "SphereComponent.h"

class LightVolumeComponent : public SphereComponent
{
protected:
    CD3D11_RASTERIZER_DESC CreateRasterizerStateDesc() override;
public:
    explicit LightVolumeComponent(Game* g);
    void SetSize(float size);
    void Draw() override;
};
