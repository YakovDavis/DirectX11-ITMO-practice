#include "SolarGame.h"
#include "Davork/Keys.h"
#include <string>

#include "CameraController.h"
#include "GameComponents/SphereComponent.h"

using namespace DirectX;
using namespace SimpleMath;

SolarGame::SolarGame() : Game(L"Solar Game", 800, 800), controller(this)
{
	//srand(static_cast<unsigned>(time(nullptr)));

	// Sun
	CelestialBodyDesc sunDesc = {};
	sunDesc.SpinSpeed = 1.0f;
	sunDesc.Radius = 2.0f;
	sunDesc.PrimaryColor = Vector4(236.f / 255.f, 1.f, 134.f / 255.f, 1.f);
	sunDesc.SecondaryColor = Vector4(153.f / 255.f, 76.f / 255.f, 0.f, 1.f);
	celestialBodies.insert( {"sun", new CelestialBodyComponent(this, sunDesc)} );

	// Mercury
	CelestialBodyDesc mercuryDesc = {};
	mercuryDesc.SpinSpeed = 1.0f;
	mercuryDesc.Radius = 0.7f;
	mercuryDesc.OrbitAnchor = celestialBodies["sun"];
	mercuryDesc.OrbitRadius = 4.0f;
	mercuryDesc.OrbitSpeed = 2.0f;
	celestialBodies.insert( {"mercury", new CelestialBodyComponent(this, mercuryDesc)} );

	// Venus
	CelestialBodyDesc venusDesc = {};
	venusDesc.SpinSpeed = -0.8f;
	venusDesc.Radius = 1.0f;
	venusDesc.OrbitAnchor = celestialBodies["sun"];
	venusDesc.OrbitRadius = 8.0f;
	venusDesc.OrbitSpeed = 1.0f;
	venusDesc.PrimaryColor = Vector4(51.f / 255.f, 25.f / 255.f, 0.0f / 255.f, 1.f);
	venusDesc.SecondaryColor = Vector4(153.f / 255.f, 76.f / 255.f, 0.0f / 255.f, 1.f);
	celestialBodies.insert( {"venus", new CelestialBodyComponent(this, venusDesc)} );

	// Earth
	CelestialBodyDesc earthDesc = {};
	earthDesc.SpinSpeed = 0.8f;
	earthDesc.Radius = 1.0f;
	earthDesc.OrbitAnchor = celestialBodies["sun"];
	earthDesc.OrbitRadius = 11.0f;
	earthDesc.OrbitSpeed = 0.8f;
	earthDesc.PrimaryColor = Vector4(0.0f, 0.0f, 0.5f, 1.0f);
	earthDesc.SecondaryColor = Vector4(0.0f, 0.7f, 0.0f, 1.0f);
	celestialBodies.insert( {"earth", new CelestialBodyComponent(this, earthDesc)} );

	// Moon
	CelestialBodyDesc moonDesc = {};
	moonDesc.SpinSpeed = 0.8f;
	moonDesc.Radius = 0.3f;
	moonDesc.OrbitAnchor = celestialBodies["earth"];
	moonDesc.OrbitRadius = 2.0f;
	moonDesc.OrbitSpeed = 0.8f;
	celestialBodies.insert( {"moon", new CelestialBodyComponent(this, moonDesc)} );

	// Adding all bodies to Components vector
	for (const std::pair<const std::string, CelestialBodyComponent*>& n : celestialBodies)
		Components.push_back(n.second);

	// Camera controller binding
	InputDev->MouseMove.AddRaw(&controller, &CameraController::OnMouseMove);
}
