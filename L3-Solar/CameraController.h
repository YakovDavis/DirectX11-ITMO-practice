#pragma once
#include "Davork/InputDevice.h"
#include "GameComponents/CelestialBodyComponent.h"

class Game;
class SolarGame;

class CameraController
{
public:
    SolarGame* sGame;
    CelestialBodyComponent* targetBody;
    DirectX::SimpleMath::Vector3 up;
    DirectX::SimpleMath::Vector3 relativePos;
    Game* game;
    bool OrbitMode;
    // fps camera
    float yaw;
    float pitch;
    float speed;
    // end of fps camera
    explicit CameraController(Game* g);
    void OnMouseMove(const InputDevice::MouseMoveEventArgs& args);
    void Update();
};
