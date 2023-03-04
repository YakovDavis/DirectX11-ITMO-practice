#include "SolarGame.h"
#include "Davork/Keys.h"
#include <string>

#include "CameraController.h"
#include "GameComponents/SphereComponent.h"

SolarGame::SolarGame() : Game(L"Solar Game", 800, 800), controller(this)
{
	//srand(static_cast<unsigned>(time(nullptr)));
	CelestialBodyDesc desc = {};
	sun = new CelestialBodyComponent(this, desc);
    Components.push_back(sun);
	InputDev->MouseMove.AddRaw(&controller, &CameraController::OnMouseMove);
}
