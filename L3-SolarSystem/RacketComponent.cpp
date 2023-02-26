#include "RacketComponent.h"
#include "Game.h"

using namespace DirectX;

RacketComponent::RacketComponent(Game* g) : QuadComponent(g)
{
	SimpleMath::Vector4 pointsTmp[8] = {
		SimpleMath::Vector4(0.01f, 0.1f, 0.5f, 1.0f),	SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		SimpleMath::Vector4(-0.01f, -0.1f, 0.5f, 1.0f),	SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		SimpleMath::Vector4(0.01f, -0.1f, 0.5f, 1.0f),	SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		SimpleMath::Vector4(-0.01f, 0.1f, 0.5f, 1.0f),	SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
	};
	std::swap(points, pointsTmp);
	Speed = 1.7f;
	CollisionBox = SimpleMath::Rectangle(-0.01f * 800, -0.1f * 800, 0.02f * 800, 0.2f * 800);
}

void RacketComponent::Update()
{
	CollisionBox.x = -0.01f * 800 + offset.x * 800;
	CollisionBox.y = -0.1f * 800 + offset.y * 800;
	QuadComponent::Update();
}
