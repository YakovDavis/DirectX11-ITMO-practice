#pragma once
#include "Game.h"
#include "QuadComponent.h"

class PongGame : public Game
{
protected:
	void Update() override;
	DirectX::SimpleMath::Vector2 pos;
	QuadComponent* r1;
public:
	PongGame();
	~PongGame();
};
