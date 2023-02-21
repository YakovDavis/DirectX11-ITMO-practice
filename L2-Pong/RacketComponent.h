#pragma once
#include "QuadComponent.h"
#include "SimpleMath.h"

class RacketComponent :
    public QuadComponent
{
public:
    DirectX::SimpleMath::Rectangle CollisionBox;
    float Speed;
    RacketComponent(Game* g);
    ~RacketComponent();
    void Update() override;
};

