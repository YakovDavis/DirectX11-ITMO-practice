#pragma once
#include "QuadComponent.h"
#include "SimpleMath.h"

class RacketComponent final :
    public QuadComponent
{
public:
    DirectX::SimpleMath::Rectangle CollisionBox;
    float Speed;
    explicit RacketComponent(Game* g);
    void Update() override;
};

