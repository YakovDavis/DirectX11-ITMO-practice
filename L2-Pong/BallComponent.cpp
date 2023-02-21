#include "BallComponent.h"
#include "PongGame.h"

using namespace DirectX;

void BallComponent::GenRndDirection()
{
	SimpleMath::Vector2::Lerp(SimpleMath::Vector2(1.0f, 1.0f), SimpleMath::Vector2(1.0f, -1.0f), (float)rand() / RAND_MAX, Direction);
	if ((float)rand() / RAND_MAX < 0.5f)
		Direction.x *= -1;
	Direction.Normalize();
}

BallComponent::BallComponent(Game* g) : QuadComponent(g)
{
	pGame = static_cast<PongGame*>(g);
	SimpleMath::Vector4 pointsTmp[8] = {
		SimpleMath::Vector4(0.01f, 0.01f, 0.5f, 1.0f),	SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		SimpleMath::Vector4(-0.01f, -0.01f, 0.5f, 1.0f),	SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		SimpleMath::Vector4(0.01f, -0.01f, 0.5f, 1.0f),	SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		SimpleMath::Vector4(-0.01f, 0.01f, 0.5f, 1.0f),	SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
	};
	std::swap(points, pointsTmp);
	Speed = 1.2f;
	GenRndDirection();
	CollisionBox = SimpleMath::Rectangle(-0.01f * 800, -0.01f * 800, 0.02f * 800, 0.02f * 800);
}

BallComponent::~BallComponent()
{
}

void BallComponent::Update()
{
	if (pGame->state == PONG_STATE_NORMAL)
	{
		CollisionBox.x = -0.01f * 800 + offset.x * 800;
		CollisionBox.y = -0.01f * 800 + offset.y * 800;
		auto nextPos = GetPosition() + Direction * Speed * game->DeltaTime;
		if (nextPos.y > 1.0f)
			Direction.y = -abs(Direction.y);
		if (nextPos.y < -1.0f)
			Direction.y = abs(Direction.y);
		if (nextPos.x < -1.0f || nextPos.x > 1.0f)
		{
			pGame->state = PONG_STATE_COOLDOWN;
			pGame->ContinueTime = std::chrono::steady_clock::now() + std::chrono::seconds(1);
			pGame->IncScore(nextPos.x > 0.0f);
			nextPos.x = 0.0f;
			nextPos.y = 0.0f;
			CollisionBox.x = -0.01f * 800;
			CollisionBox.y = -0.01f * 800;
			GenRndDirection();
		}
		if (pGame->racket1->CollisionBox.Contains(nextPos * 800))
		{
			SimpleMath::Vector2::Lerp(SimpleMath::Vector2(1.f, -1.f), SimpleMath::Vector2(1.f, 1.f), abs(GetY() - pGame->racket1->GetY() + 0.1f) / 0.2f, Direction);
			Direction.Normalize();
			nextPos = GetPosition() + Direction * Speed * game->DeltaTime;
		}
		if (pGame->racket2->CollisionBox.Contains(nextPos * 800))
		{
			SimpleMath::Vector2::Lerp(SimpleMath::Vector2(-1.f, -1.f), SimpleMath::Vector2(-1.f, 1.f), abs(GetY() - pGame->racket2->GetY() + 0.1f) / 0.2f, Direction);
			Direction.Normalize();
			nextPos = GetPosition() + Direction * Speed * game->DeltaTime;
		}
		SetPosition(nextPos);
	}
	QuadComponent::Update();
}
