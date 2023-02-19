#include "PongGame.h"

void PongGame::Update()
{
	Game::Update();
	if (InputDev->IsKeyDown(38))
		r1->SetY(r1->GetY() + DeltaTime);
	if (InputDev->IsKeyDown(40))
		r1->SetY(r1->GetY() - DeltaTime);
}

PongGame::PongGame() : Game(L"MyGame", 800, 800)
{
	r1 = new QuadComponent(this);
	Components.push_back(r1);
}

PongGame::~PongGame()
{
	for (auto c : Components)
	{
		delete c;
	}
}
