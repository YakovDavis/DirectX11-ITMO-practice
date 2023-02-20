#pragma once
#include "Game.h"
#include "RacketComponent.h"
#include "BallComponent.h"

class PongGame : public Game
{
protected:
	void Update() override;
	DirectX::SimpleMath::Vector2 pos;
	RacketComponent* racket1;
	RacketComponent* racket2;
	BallComponent* ball;
public:
	PongGame();
	~PongGame();
};
