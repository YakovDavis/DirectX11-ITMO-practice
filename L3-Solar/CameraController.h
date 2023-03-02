#pragma once
#include "Davork/InputDevice.h"

class Game;

class CameraController
{
public:
    Game* game;
    bool OrbitMode;
    explicit CameraController(Game* g);
    void OnMouseMove(const InputDevice::MouseMoveEventArgs& args);
    void ProcessInput();
};
