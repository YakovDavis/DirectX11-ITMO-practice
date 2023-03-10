#include "KatamariBall.h"
#include "../Davork/Game.h"
#include "../KatamariGame.h"
#include "KatamariFurnitureComponent.h"

using namespace DirectX;
using namespace SimpleMath;

void KatamariBall::UpdateSize(float absorbedSize)
{
    float tmp = sqrtf(collision.Radius * collision.Radius + absorbedSize * absorbedSize);
    collision.Radius = tmp;
    position.y = tmp;
    rotationMaxSpeed = 0.1f / tmp;
    moveMaxSpeed = 8.0f * tmp;
#ifdef _DEBUG
    std::cout << tmp << std::endl;
#endif
}

KatamariBall::KatamariBall(Game* game) : SphereComponent(game, 1.0f, 32, 32, L"Textures/ac.dds"),
                                         outline(new SphereComponent(game, 1.001f, 32, 32, L"Textures/ac.dds")), rotationDrag(0.14f), rotationMaxSpeed(0.1f), moveMaxSpeed(8.0f), moveDrag(5.0f), savedRot(Quaternion::Identity),
                                         velocity(Vector3::Zero), collision(position, 1.0f)
{
    kGame = dynamic_cast<KatamariGame*>(game);
}

void KatamariBall::Update()
{
    collision.Center = position;
    
    for (auto furn : kGame->furniture)
    {
        if (collision.Intersects(furn->collision) && !furn->isPickedUp && collision.Radius > furn->collision.Radius)
        {
            furn->isPickedUp = true;
            furn->kb = this;
            furn->initRelPos = furn->GetPosition() - GetPosition();
            rotation.Inverse(furn->invKbRot);
            UpdateSize(furn->collision.Radius);
        }
    }
    
    savedRot.RotateTowards(Quaternion::Identity, rotationDrag * game->DeltaTime);
    rotation *= savedRot;
    velocity *= 1.0f - moveDrag * game->DeltaTime;
    
    SphereComponent::Update();

    position += velocity * game->DeltaTime;
}

void KatamariBall::SetDirection(Vector3 dir)
{
    Vector3 tmp = Vector3(dir.x, 0.0f, dir.z);
    tmp.Normalize();
    Quaternion q = Quaternion::CreateFromAxisAngle(tmp.Cross(Vector3::Up), -rotationMaxSpeed);
    float f = Quaternion::Angle(Quaternion::Identity, savedRot) / 0.1f;
    savedRot *= Quaternion::Lerp(q, Quaternion::Identity, f);
    velocity = tmp * moveMaxSpeed;
}
