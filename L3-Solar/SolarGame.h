#pragma once
#include "CameraController.h"
#include "Davork/Camera.h"
#include "Davork/Game.h"
#include "GameComponents/CelestialBodyComponent.h"

class SolarGame : public Game
{
public:
	CameraController controller;
	CelestialBodyComponent* sun;
	SolarGame();
};
