#include "CameraController.h"

#include "Davork/Game.h"

using namespace DirectX;
using namespace SimpleMath;

CameraController::CameraController(Game* g) : game(g), OrbitMode(true), up(Vector3::Up)
{
}

void CameraController::OnMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
    if (OrbitMode)
    {
        if (game->InputDev->IsKeyDown(Keys::LeftButton))
        {
            auto right = game->Camera->Position.Cross(up);
            auto qua = Quaternion::CreateFromAxisAngle(up, 0.01f * game->InputDev->MouseOffset.x) * Quaternion::CreateFromAxisAngle(right, - 0.01f * game->InputDev->MouseOffset.y);
            game->Camera->Position = XMVector4Transform(game->Camera->Position, Matrix::CreateFromQuaternion(qua));
            up = XMVector4Transform(up, Matrix::CreateFromQuaternion(qua));
            //vec = XMVector4Transform(vec, Matrix::CreateFromYawPitchRoll(0.01f * game->InputDev->MouseOffset.x, 0.01f * game->InputDev->MouseOffset.y, 0.0f));
            //up = XMVector4Transform(up, Matrix::CreateFromYawPitchRoll(0.01f * game->InputDev->MouseOffset.x, 0.01f * game->InputDev->MouseOffset.y, 0.0f));
            game->Camera->Up = up;
        }
        game->Camera->Position *= 1 - 0.001f * game->InputDev->MouseWheelDelta;
    }
}

void CameraController::ProcessInput()
{
}


