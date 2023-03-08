#include "KatamariBall.h"

#include "../Davork/Game.h"

using namespace DirectX;
using namespace SimpleMath;

KatamariBall::KatamariBall(Game* game) : SphereComponent(game, 1.0f, 32, 32, L"Textures/compost.dds"),
    savedRot(Quaternion::Identity), rotationDrag(0.15f), rotationMaxSpeed(0.1f), velocity(Vector3::Zero), moveMaxSpeed(10.0f), moveDrag(5.0f)
{
}

void KatamariBall::Update()
{
    savedRot.RotateTowards(Quaternion::Identity, rotationDrag * game->DeltaTime);
    rotation *= savedRot;
    velocity *= 1.0f - moveDrag * game->DeltaTime;
    position += velocity * game->DeltaTime;
    SphereComponent::Update();
}

void KatamariBall::SetDirection(Vector3 dir)
{
    Vector3 tmp = Vector3(dir.x, 0.0f, dir.z);
    tmp.Normalize();
    savedRot =  Quaternion::CreateFromAxisAngle(tmp.Cross(Vector3::Up), -rotationMaxSpeed);
    velocity = tmp * moveMaxSpeed;
}
