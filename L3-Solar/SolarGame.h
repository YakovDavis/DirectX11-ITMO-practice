#pragma once
#include "Davork/Camera.h"
#include "Davork/Game.h"
#include "GameComponents/SphereComponent.h"

class SolarGame : public Game
{
public:
	SphereComponent* sphere;
	SolarGame();
};
