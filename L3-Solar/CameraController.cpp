#include "CameraController.h"

#include "Davork/Game.h"

using namespace DirectX;
using namespace SimpleMath;

CameraController::CameraController(Game* g) : game(g), OrbitMode(true)
{
}

void CameraController::OnMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
    if (OrbitMode)
    {
        if (game->InputDev->IsKeyDown(Keys::LeftButton))
        {
            auto vec = Vector4(game->Camera->Position.x, game->Camera->Position.y, game->Camera->Position.z, 1.0f);
            vec = XMVector4Transform(vec, Matrix::CreateFromYawPitchRoll(0.01f * game->InputDev->MouseOffset.x, 0.01f * game->InputDev->MouseOffset.y, 0.0f));
            game->Camera->Position = Vector3(vec.x, vec.y, vec.z);
            auto tmp = (game->Camera->Position - game->Camera->Target);
            tmp = tmp.Cross(Vector3::Up);
            tmp = tmp.Cross(game->Camera->Position - game->Camera->Target);
            tmp.Normalize();
            game->Camera->Up = tmp;
        }
        if (game->InputDev->IsKeyDown(Keys::RightButton))
        {
            game->Camera->Position *= 1 + 0.01f * game->InputDev->MouseOffset.y;
        }
    }
}

void CameraController::ProcessInput()
{
}


