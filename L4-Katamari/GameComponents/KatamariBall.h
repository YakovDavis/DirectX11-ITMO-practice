#pragma once
#include "../Davork/GameComponents/SphereComponent.h"

class KatamariBall :  public SphereComponent
{
public:
    float rotationDrag;
    float rotationMaxSpeed;
    float moveMaxSpeed;
    float moveDrag;
    DirectX::SimpleMath::Quaternion savedRot;
    DirectX::SimpleMath::Vector3 velocity;
    KatamariBall(Game* game);
    void Update() override;
    void SetDirection(DirectX::SimpleMath::Vector3 dir);
};
