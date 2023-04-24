#pragma once

#include "Davork/Game.h"
#include "Davork/OrbitCameraController.h"

class KatamariFurnitureComponent;
class KatamariBall;
class ParticleSystem;

class KatamariGame : public Game
{
protected:
    KatamariBall* ball;
    ParticleSystem* cloudPartSys;
    ParticleSystem* rainPartSys;
    OrbitCameraController* orbitCameraController{};
public:
    std::vector<KatamariFurnitureComponent*> furniture {};
    KatamariGame();
    void Update() override;
};
