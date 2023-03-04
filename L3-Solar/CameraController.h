#pragma once
#include "Davork/InputDevice.h"

class Game;

class CameraController
{
public:
    DirectX::SimpleMath::Vector3 up;
    Game* game;
    bool OrbitMode;
    explicit CameraController(Game* g);
    void OnMouseMove(const InputDevice::MouseMoveEventArgs& args);
    void ProcessInput();
};
