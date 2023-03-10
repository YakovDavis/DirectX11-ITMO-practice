#pragma once

#include "../Davork/GameComponents/SphereComponent.h"

class KatamariGame;

class KatamariBall :  public SphereComponent
{
protected:
    KatamariGame* kGame;
    void UpdateSize(float absorbedSize);
public:
    SphereComponent* outline;
    float rotationDrag;
    float rotationMaxSpeed;
    float moveMaxSpeed;
    float moveDrag;
    DirectX::SimpleMath::Quaternion savedRot;
    DirectX::SimpleMath::Vector3 velocity;
    DirectX::BoundingSphere collision;
    KatamariBall(Game* game);
    void Update() override;
    void SetDirection(DirectX::SimpleMath::Vector3 dir);
};
