#pragma once

#include "Davork/FPSCameraController.h"
#include "Davork/Game.h"

class KatamariGame : public Game
{
protected:
    FPSCameraController* fpsCameraController{};
public:
    KatamariGame();
    void Update() override;
};
