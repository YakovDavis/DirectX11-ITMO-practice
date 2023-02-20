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
	Speed = 1.0f;
}

RacketComponent::~RacketComponent()
{
}
