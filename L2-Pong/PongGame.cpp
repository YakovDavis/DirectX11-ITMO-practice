#include "PongGame.h"

void PongGame::Update()
{
	Game::Update();
	if (InputDev->IsKeyDown(38) && r2->GetY() < 0.8f)
		r2->SetY(r2->GetY() + DeltaTime);
	if (InputDev->IsKeyDown(40) && r2->GetY() > -0.8f)
		r2->SetY(r2->GetY() - DeltaTime);
}

PongGame::PongGame() : Game(L"MyGame", 800, 600)
{
	r1 = new RacketComponent(this);
	r1->SetX(-0.9f);
	r2 = new RacketComponent(this);
	r2->SetX(0.9f);
	Components.push_back(r1);
	Components.push_back(r2);
}

PongGame::~PongGame()
{
}
