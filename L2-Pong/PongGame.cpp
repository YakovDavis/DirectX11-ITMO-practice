#include "PongGame.h"

void PongGame::Update()
{
	Game::Update();
	if (InputDev->IsKeyDown(38) && racket2->GetY() < 0.8f)
		racket2->SetY(racket2->GetY() + DeltaTime * racket2->Speed);
	if (InputDev->IsKeyDown(40) && racket2->GetY() > -0.8f)
		racket2->SetY(racket2->GetY() - DeltaTime * racket2->Speed);
	ball->SetPosition(ball->GetPosition() + ball->Direction * ball->Speed * DeltaTime);
}

PongGame::PongGame() : Game(L"MyGame", 800, 800)
{
	racket1 = new RacketComponent(this);
	racket1->SetX(-0.9f);
	racket2 = new RacketComponent(this);
	racket2->SetX(0.9f);
	ball = new BallComponent(this);
	Components.push_back(racket1);
	Components.push_back(racket2);
	Components.push_back(ball);
}

PongGame::~PongGame()
{
}
