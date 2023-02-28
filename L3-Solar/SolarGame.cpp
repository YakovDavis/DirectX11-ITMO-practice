#include "SolarGame.h"
#include "Davork/Keys.h"
#include <string>
#include "GameComponents/SphereComponent.h"

SolarGame::SolarGame() : Game(L"Solar Game", 800, 800)
{
	//srand(static_cast<unsigned>(time(nullptr)));
	sphere = new SphereComponent(this, 1.0f, 5, 5);
    Components.push_back(sphere);
}
