﻿#include "KatamariFurnitureComponent.h"

#include "KatamariBall.h"

using namespace DirectX;
using namespace SimpleMath;

KatamariFurnitureComponent::KatamariFurnitureComponent(Game* g, const std::string fileNameModel, const wchar_t* fileNameTexture, float katSize, Vector3 collOffset) :
    MeshRenderComponent(g, fileNameModel, fileNameTexture), collision(position + collOffset, katSize), isPickedUp(false), originCollisionOffset(collOffset)
{
}

void KatamariFurnitureComponent::Update()
{
    if (isPickedUp)
    {
        if (kb)
        {
            SetPosition(kb->GetPosition() + Vector3::Transform(initRelPos, invKbRot * kb->GetRotation()));
            SetRotation(invKbRot * kb->GetRotation());
        }
    }
    MeshRenderComponent::Update();
}

void KatamariFurnitureComponent::SetPosition(Vector3 p)
{
    collision.Center = p + originCollisionOffset;
    MeshRenderComponent::SetPosition(p);
}
