#pragma once
#include "QuadComponent.h"
#include "BallComponent.h"

class BallComponent :
    public QuadComponent
{
public:
    float Speed;
    DirectX::SimpleMath::Vector2 Direction;
    BallComponent(Game* g);
    ~BallComponent();
};

