#include "OrbitCameraController.h"
#include "Game.h"

using namespace DirectX;
using namespace SimpleMath;

OrbitCameraController::OrbitCameraController(Game* g, Camera* c, GameComponent* t) : game(g), camera(c), target(t),
    rotation(Quaternion::Identity), radius(2.0f), sensitivityX(0.004f), sensitivityY(0.004f), isLMBActivated(true)
{
}

void OrbitCameraController::OnMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
    if (game->InputDevice->IsKeyDown(Keys::LeftButton) || !isLMBActivated)
    {
        rotation *= Quaternion::CreateFromAxisAngle(Vector3::Up, sensitivityX * game->InputDevice->MouseOffset.x);
        Vector3 tmp = GetUp();
        tmp = tmp.Cross(target->GetPosition() - camera->Position);
        rotation *= Quaternion::CreateFromAxisAngle(tmp, sensitivityY * game->InputDevice->MouseOffset.y);

        camera->Up = Vector3::Transform(Vector3::Up, rotation);
        camera->Target = game->Camera->Position;
        camera->Target += Vector3::Transform(Vector3::Forward, rotation);
    }
}

void OrbitCameraController::Update()
{
    camera->Position = target->GetPosition() - GetForward() * radius;
}

Vector3 OrbitCameraController::GetForward() const
{
    return Vector3::Transform(Vector3::Forward, rotation);
}

Vector3 OrbitCameraController::GetUp() const
{
    return Vector3::Transform(Vector3::Up, rotation);
}
