#pragma once

#include "Davork/Game.h"
#include "Davork/OrbitCameraController.h"

class KatamariBall;

class KatamariGame : public Game
{
protected:
    KatamariBall* ball;
    OrbitCameraController* orbitCameraController{};
public:
    KatamariGame();
    void Update() override;
};
