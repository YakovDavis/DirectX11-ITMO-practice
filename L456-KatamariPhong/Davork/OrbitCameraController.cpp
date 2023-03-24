﻿#include "OrbitCameraController.h"
#include "Game.h"

using namespace DirectX;
using namespace SimpleMath;

OrbitCameraController::OrbitCameraController(Game* g, Camera* c, GameComponent* t) : game(g), camera(c), target(t),
    rotation(Quaternion::Identity), radius(10.0f), sensitivityX(0.004f), sensitivityY(0.004f), isLMBActivated(true)
{
}

void OrbitCameraController::OnMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
    if (game->InputDevice->IsKeyDown(Keys::LeftButton) || !isLMBActivated)
    {
        Vector3 tmp = Vector3::Transform(Vector3::Right, rotation);
        if ((GetForward().y < 0 || game->InputDevice->MouseOffset.y < 0) && (GetUp().y > 0.05f || game->InputDevice->MouseOffset.y > 0))
            rotation *= Quaternion::CreateFromAxisAngle(tmp, sensitivityY * game->InputDevice->MouseOffset.y);
        rotation *= Quaternion::CreateFromAxisAngle(Vector3::Up, - sensitivityX * game->InputDevice->MouseOffset.x);
    }
    if ((radius > 5.0 || game->InputDevice->MouseWheelDelta < 0) && (radius < 30.0 || game->InputDevice->MouseWheelDelta > 0))
        radius -= 0.01f * game->InputDevice->MouseWheelDelta;
}

void OrbitCameraController::Update()
{
    camera->Position = target->GetPosition() - GetForward() * radius;
    camera->Up = Vector3::Transform(Vector3::Up, rotation);
    camera->Target = target->GetPosition();
}

Vector3 OrbitCameraController::GetForward() const
{
    return Vector3::Transform(Vector3::Forward, rotation);
}

Vector3 OrbitCameraController::GetUp() const
{
    return Vector3::Transform(Vector3::Up, rotation);
}
