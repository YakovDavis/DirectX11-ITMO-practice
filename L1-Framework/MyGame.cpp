#include "MyGame.h"

MyGame::MyGame() : Game(L"MyGame", 800, 800)
{

}

MyGame::~MyGame()
{
	for (auto c : Components)
	{
		delete c;
	}
}

void MyGame::DestroyResources()
{
	for (auto c : Components)
	{
		c->DestroyResources();
	}
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

void MyGame::EndFrame()
{
}

void MyGame::Initialize()
{
	Components.push_back(new TriangleComponent(this));
	Components.push_back(new TriangleComponent(this));

	for (auto c : Components)
	{
		c->Initialize();
	}
}

void MyGame::PrepareFrame()
{
}

void MyGame::PrepareResources()
{
}

void MyGame::Update()
{
}

void MyGame::UpdateInternal()
{
}
