﻿#include "CameraController.h"
#include "SolarGame.h"

using namespace DirectX;
using namespace SimpleMath;

CameraController::CameraController(Game* g) : targetBody(nullptr), up(Vector3::Up), game(g), OrbitMode(true), speed(0.2f)
{
    sGame = dynamic_cast<SolarGame*>(g);
    relativePos = game->Camera->Position;
}

void CameraController::OnMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
    if (OrbitMode)
    {
        if (game->InputDevice->IsKeyDown(Keys::LeftButton))
        {
            auto right = relativePos.Cross(up);
            auto qua = Quaternion::CreateFromAxisAngle(up, 0.005f * game->InputDevice->MouseOffset.x) * Quaternion::CreateFromAxisAngle(right, - 0.005f * game->InputDevice->MouseOffset.y);
            relativePos = XMVector4Transform(relativePos, Matrix::CreateFromQuaternion(qua));
            up = XMVector4Transform(up, Matrix::CreateFromQuaternion(qua));
            //vec = XMVector4Transform(vec, Matrix::CreateFromYawPitchRoll(0.01f * game->InputDev->MouseOffset.x, 0.01f * game->InputDev->MouseOffset.y, 0.0f));
            //up = XMVector4Transform(up, Matrix::CreateFromYawPitchRoll(0.01f * game->InputDev->MouseOffset.x, 0.01f * game->InputDev->MouseOffset.y, 0.0f));
            game->Camera->Up = up;
        }
        relativePos *= 1 - 0.001f * game->InputDevice->MouseWheelDelta;
    }
    else
    {
        if (game->InputDevice->IsKeyDown(Keys::LeftButton))
        {
            yaw -= 0.004f * game->InputDevice->MouseOffset.x;
            while (yaw < -XM_2PI)
                yaw += XM_2PI;
            while (yaw < XM_2PI)
                yaw += XM_2PI;
            pitch -= 0.004f * game->InputDevice->MouseOffset.y;
            game->Camera->Up = XMVector4Transform(Vector3::Up, Matrix::CreateFromYawPitchRoll(yaw, pitch, 0.0f));
            game->Camera->Target = game->Camera->Position;
            game->Camera->Target += XMVector4Transform(Vector3::Forward, Matrix::CreateFromYawPitchRoll(yaw, pitch, 0.0f));
        }
    }
}

void CameraController::Update()
{
    if (game->InputDevice->IsKeyDown(Keys::F1))
        game->Camera->IsOrthographic = false;
    if (game->InputDevice->IsKeyDown(Keys::F2))
        game->Camera->IsOrthographic = true;
    
    if (OrbitMode && (game->InputDevice->IsKeyDown(Keys::W) || game->InputDevice->IsKeyDown(Keys::A) || game->InputDevice->IsKeyDown(Keys::S)
        || game->InputDevice->IsKeyDown(Keys::D) || game->InputDevice->IsKeyDown(Keys::E) || game->InputDevice->IsKeyDown(Keys::Z)))
    {
        OrbitMode = false;
        auto q = Quaternion::LookRotation(game->Camera->Target - game->Camera->Position, game->Camera->Up);
        auto v = q.ToEuler();
        yaw = v.y;
        pitch = 0.0f;
        targetBody = nullptr;
    }
    if (game->InputDevice->IsKeyDown(Keys::D0))
    {
        OrbitMode = true;
        targetBody = sGame->celestialBodies["sun"];
        game->Camera->Position = Vector3::Forward * 20.0f;
        game->Camera->Up = Vector3::Up;
        up = Vector3::Up;
    }
    if (game->InputDevice->IsKeyDown(Keys::D1))
    {
        OrbitMode = true;
        targetBody = sGame->celestialBodies["mercury"];
        game->Camera->Position = Vector3::Forward * 20.0f;
        game->Camera->Up = Vector3::Up;
        up = Vector3::Up;
    }
    if (game->InputDevice->IsKeyDown(Keys::D2))
    {
        OrbitMode = true;
        targetBody = sGame->celestialBodies["venus"];
        game->Camera->Position = Vector3::Forward * 20.0f;
        game->Camera->Up = Vector3::Up;
        up = Vector3::Up;
    }
    if (game->InputDevice->IsKeyDown(Keys::D3))
    {
        OrbitMode = true;
        targetBody = sGame->celestialBodies["earth"];
        game->Camera->Position = Vector3::Forward * 20.0f;
        game->Camera->Up = Vector3::Up;
        up = Vector3::Up;
    }
    if (game->InputDevice->IsKeyDown(Keys::D4))
    {
        OrbitMode = true;
        targetBody = sGame->celestialBodies["moon"];
        game->Camera->Position = Vector3::Forward * 20.0f;
        game->Camera->Up = Vector3::Up;
        up = Vector3::Up;
    }
    if (game->InputDevice->IsKeyDown(Keys::D5))
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

    if (!OrbitMode)
    {
        if (game->InputDevice->IsKeyDown(Keys::W))
        {
            Vector3 tmp = XMVector4Transform(Vector3::Forward, Matrix::CreateFromYawPitchRoll(yaw, pitch, 0.0f));
            tmp.Normalize();
            game->Camera->Position += speed * tmp;
        }
        if (game->InputDevice->IsKeyDown(Keys::S))
        {
            Vector3 tmp = XMVector4Transform(Vector3::Backward, Matrix::CreateFromYawPitchRoll(yaw, pitch, 0.0f));
            tmp.Normalize();
            game->Camera->Position += speed * tmp;
        }
        if (game->InputDevice->IsKeyDown(Keys::A))
        {
            Vector3 tmp = XMVector4Transform(Vector3::Left, Matrix::CreateFromYawPitchRoll(yaw, pitch, 0.0f));
            tmp.Normalize();
            game->Camera->Position += speed * tmp;
        }
        if (game->InputDevice->IsKeyDown(Keys::D))
        {
            Vector3 tmp = XMVector4Transform(Vector3::Right, Matrix::CreateFromYawPitchRoll(yaw, pitch, 0.0f));
            tmp.Normalize();
            game->Camera->Position += speed * tmp;
        }
        if (game->InputDevice->IsKeyDown(Keys::E))
        {
            game->Camera->Position += speed * Vector3::Up;
        }
        if (game->InputDevice->IsKeyDown(Keys::Z))
        {
            game->Camera->Position += speed * Vector3::Down;
        }
        game->Camera->Up = XMVector4Transform(Vector3::Up, Matrix::CreateFromYawPitchRoll(yaw, pitch, 0.0f));
        game->Camera->Target = game->Camera->Position;
        game->Camera->Target += XMVector4Transform(Vector3::Forward, Matrix::CreateFromYawPitchRoll(yaw, pitch, 0.0f));
    }
}

