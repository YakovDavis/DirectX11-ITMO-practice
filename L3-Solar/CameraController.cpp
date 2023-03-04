#include "CameraController.h"
#include "SolarGame.h"

using namespace DirectX;
using namespace SimpleMath;

CameraController::CameraController(Game* g) : targetBody(nullptr), up(Vector3::Up), game(g), OrbitMode(true)
{
    sGame = dynamic_cast<SolarGame*>(g);
    relativePos = game->Camera->Position;
}

void CameraController::OnMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
    if (OrbitMode)
    {
        if (game->InputDev->IsKeyDown(Keys::LeftButton))
        {
            auto right = relativePos.Cross(up);
            auto qua = Quaternion::CreateFromAxisAngle(up, 0.01f * game->InputDev->MouseOffset.x) * Quaternion::CreateFromAxisAngle(right, - 0.01f * game->InputDev->MouseOffset.y);
            relativePos = XMVector4Transform(relativePos, Matrix::CreateFromQuaternion(qua));
            up = XMVector4Transform(up, Matrix::CreateFromQuaternion(qua));
            //vec = XMVector4Transform(vec, Matrix::CreateFromYawPitchRoll(0.01f * game->InputDev->MouseOffset.x, 0.01f * game->InputDev->MouseOffset.y, 0.0f));
            //up = XMVector4Transform(up, Matrix::CreateFromYawPitchRoll(0.01f * game->InputDev->MouseOffset.x, 0.01f * game->InputDev->MouseOffset.y, 0.0f));
            game->Camera->Up = up;
        }
        relativePos *= 1 - 0.001f * game->InputDev->MouseWheelDelta;
    }
}

void CameraController::Update()
{
    if (game->InputDev->IsKeyDown(Keys::D0))
    {
        OrbitMode = true;
        targetBody = sGame->celestialBodies["sun"];
        game->Camera->Position = Vector3::Forward * 20.0f;
        game->Camera->Up = Vector3::Up;
        up = Vector3::Up;
    }
    if (game->InputDev->IsKeyDown(Keys::D1))
    {
        OrbitMode = true;
        targetBody = sGame->celestialBodies["mercury"];
        game->Camera->Position = Vector3::Forward * 20.0f;
        game->Camera->Up = Vector3::Up;
        up = Vector3::Up;
    }
    if (game->InputDev->IsKeyDown(Keys::D2))
    {
        OrbitMode = true;
        targetBody = sGame->celestialBodies["venus"];
        game->Camera->Position = Vector3::Forward * 20.0f;
        game->Camera->Up = Vector3::Up;
        up = Vector3::Up;
    }
    if (game->InputDev->IsKeyDown(Keys::D3))
    {
        OrbitMode = true;
        targetBody = sGame->celestialBodies["earth"];
        game->Camera->Position = Vector3::Forward * 20.0f;
        game->Camera->Up = Vector3::Up;
        up = Vector3::Up;
    }
    if (game->InputDev->IsKeyDown(Keys::D4))
    {
        OrbitMode = true;
        targetBody = sGame->celestialBodies["moon"];
        game->Camera->Position = Vector3::Forward * 20.0f;
        game->Camera->Up = Vector3::Up;
        up = Vector3::Up;
    }
    if (game->InputDev->IsKeyDown(Keys::D5))
    {
        OrbitMode = true;
        targetBody = sGame->celestialBodies["mars"];
        game->Camera->Position = Vector3::Forward * 20.0f;
        game->Camera->Up = Vector3::Up;
        up = Vector3::Up;
    }

    if (OrbitMode && targetBody)
    {
        game->Camera->Target = targetBody->position;
        game->Camera->Position = targetBody->position + relativePos;
    }

    if (OrbitMode && !targetBody)
    {
        game->Camera->Target = Vector3::Zero;
        game->Camera->Position = relativePos;
    }
}

