#include "MyGame.h"

MyGame::MyGame() : Game(L"MyGame", 800, 800)
{
	Components.push_back(new QuadComponent(this));
}

MyGame::~MyGame()
{
}

void MyGame::Draw()
{
	float color[] = { TotalTime, 0.1f, 0.1f, 1.0f };
	Context->ClearRenderTargetView(RenderView, color);

	for (auto c : Components)
	{
		c->Draw();
	}
}
