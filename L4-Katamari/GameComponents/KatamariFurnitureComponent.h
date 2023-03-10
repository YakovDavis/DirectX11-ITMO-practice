﻿#pragma once
#include "../Davork/GameComponents/MeshRenderComponent.h"

class KatamariFurnitureComponent : public MeshRenderComponent
{
public:
    bool isPickedUp;
    KatamariFurnitureComponent(Game* g, std::string fileNameModel, const wchar_t* fileNameTexture, float katSize, DirectX::SimpleMath::Vector3 collOffset);
    DirectX::BoundingSphere collision;
    DirectX::SimpleMath::Vector3 initRelPos;
    DirectX::SimpleMath::Quaternion invKbRot;
    DirectX::SimpleMath::Vector3 originCollisionOffset;
    void Update() override;
    void SetPosition(DirectX::SimpleMath::Vector3 p) override;
};
