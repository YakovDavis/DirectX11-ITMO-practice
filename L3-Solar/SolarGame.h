#pragma once
#include "CameraController.h"
#include "Davork/Camera.h"
#include "Davork/Game.h"
#include "GameComponents/SphereComponent.h"

class SolarGame : public Game
{
public:
	CameraController controller;
	SphereComponent* sphere;
	SolarGame();
};
