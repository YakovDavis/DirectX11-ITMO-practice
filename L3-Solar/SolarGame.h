#pragma once

#include <unordered_map>
#include "CameraController.h"
#include "Davork/Camera.h"
#include "Davork/Game.h"
#include "GameComponents/CelestialBodyComponent.h"

class SolarGame : public Game
{
public:
	std::unordered_map<std::string, CelestialBodyComponent*> celestialBodies;
	CameraController controller;
	SolarGame();
	void Update() override;
};
