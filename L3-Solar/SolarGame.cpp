#include "SolarGame.h"
#include "Davork/Keys.h"
#include <string>
#include "GameComponents/SphereComponent.h"

SolarGame::SolarGame() : Game(L"Solar Game", 800, 800)
{
	//srand(static_cast<unsigned>(time(nullptr)));
	sphere = new SphereComponent(this, 1.0f, 16, 16, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.5f, 1.0f), DirectX::SimpleMath::Vector4(0.0f, 0.7f, 0.0f, 1.0f));
    Components.push_back(sphere);
}
