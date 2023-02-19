#pragma once
#include "Game.h"
#include "RacketComponent.h"

class PongGame : public Game
{
protected:
	void Update() override;
	DirectX::SimpleMath::Vector2 pos;
	RacketComponent* r1;
	RacketComponent* r2;
public:
	PongGame();
	~PongGame();
};
