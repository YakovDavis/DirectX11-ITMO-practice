#pragma once

#include "Davork/Game.h"
#include "Davork/OrbitCameraController.h"

class KatamariFurnitureComponent;
class KatamariBall;

class KatamariGame : public Game
{
protected:
    KatamariBall* ball;
    OrbitCameraController* orbitCameraController{};
public:
    std::vector<KatamariFurnitureComponent*> furniture {};
    KatamariGame();
    void Update() override;
};
