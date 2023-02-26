#pragma once
#include "QuadComponent.h"
#include "BallComponent.h"

class PongGame;

class BallComponent :
    public QuadComponent
{
private:
    PongGame* pGame;
    void GenRndDirection();
public:
    float Speed;
    DirectX::SimpleMath::Vector2 Direction;
    BallComponent(Game* g);
    void Update() override;
};

