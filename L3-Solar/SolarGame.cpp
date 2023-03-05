#include "SolarGame.h"
#include "Davork/Keys.h"
#include <string>

#include "CameraController.h"
#include "GameComponents/GridComponent.h"
#include "GameComponents/SphereComponent.h"

using namespace DirectX;
using namespace SimpleMath;

SolarGame::SolarGame() : Game(L"Solar Game", 800, 800), controller(this)
{
	//srand(static_cast<unsigned>(time(nullptr)));

	//Grid
	Components.push_back(new GridComponent(this, 1.0f, 1000));

	// Sun
	CelestialBodyDesc sunDesc = {};
	sunDesc.SpinSpeed = 1.0f;
	sunDesc.Radius = 3.0f;
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
	mercuryDesc.OrbitOrientationForward = XMVector4Transform(mercuryDesc.OrbitOrientationForward, Matrix::CreateFromAxisAngle(Vector3::Right, 7.0f * XM_PI / 180.0f));
	mercuryDesc.OrbitOrientationUp = XMVector4Transform(mercuryDesc.OrbitOrientationUp, Matrix::CreateFromAxisAngle(Vector3::Right, 7.0f * XM_PI / 180.0f));
	celestialBodies.insert( {"mercury", new CelestialBodyComponent(this, mercuryDesc)} );

	// Venus
	CelestialBodyDesc venusDesc = {};
	venusDesc.SpinSpeed = -0.8f;
	venusDesc.Radius = 1.0f;
	venusDesc.OrbitAnchor = celestialBodies["sun"];
	venusDesc.OrbitRadius = 7.0f;
	venusDesc.OrbitSpeed = 1.0f;
	venusDesc.OrbitOrientationForward = XMVector4Transform(mercuryDesc.OrbitOrientationForward, Matrix::CreateFromAxisAngle(Vector3::Right, 3.0f * XM_PI / 180.0f));
	venusDesc.OrbitOrientationUp = XMVector4Transform(mercuryDesc.OrbitOrientationUp, Matrix::CreateFromAxisAngle(Vector3::Right, 3.0f * XM_PI / 180.0f));
	venusDesc.PrimaryColor = Vector4(51.f / 255.f, 25.f / 255.f, 0.0f / 255.f, 1.f);
	venusDesc.SecondaryColor = Vector4(153.f / 255.f, 76.f / 255.f, 0.0f / 255.f, 1.f);
	celestialBodies.insert( {"venus", new CelestialBodyComponent(this, venusDesc)} );

	// Earth
	CelestialBodyDesc earthDesc = {};
	earthDesc.SpinSpeed = 0.8f;
	earthDesc.Radius = 1.0f;
	earthDesc.OrbitAnchor = celestialBodies["sun"];
	earthDesc.OrbitRadius = 10.0f;
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

	// Mars
	CelestialBodyDesc marsDesc = {};
	marsDesc.SpinSpeed = 0.8f;
	marsDesc.Radius = 0.6f;
	marsDesc.OrbitAnchor = celestialBodies["sun"];
	marsDesc.OrbitRadius = 15.0f;
	marsDesc.OrbitSpeed = 1.0f;
	marsDesc.OrbitOrientationForward = XMVector4Transform(mercuryDesc.OrbitOrientationForward, Matrix::CreateFromAxisAngle(Vector3::Right, 2.0f * XM_PI / 180.0f));
	marsDesc.OrbitOrientationUp = XMVector4Transform(mercuryDesc.OrbitOrientationUp, Matrix::CreateFromAxisAngle(Vector3::Right, 2.0f * XM_PI / 180.0f));
	marsDesc.PrimaryColor = Vector4(51.f / 255.f, 25.f / 255.f, 0.0f / 255.f, 1.f);
	marsDesc.SecondaryColor = Vector4(153.f / 255.f, 76.f / 255.f, 0.0f / 255.f, 1.f);
	celestialBodies.insert( {"mars", new CelestialBodyComponent(this, marsDesc)} );

	// Phobos
	CelestialBodyDesc phobosDesc = {};
	phobosDesc.SpinSpeed = 0.8f;
	phobosDesc.Radius = 0.1f;
	phobosDesc.OrbitAnchor = celestialBodies["mars"];
	phobosDesc.OrbitRadius = 1.0f;
	phobosDesc.OrbitSpeed = 2.0f;
	phobosDesc.OrbitOrientationForward = XMVector4Transform(phobosDesc.OrbitOrientationForward, Matrix::CreateFromAxisAngle(Vector3::Right, 10.0f * XM_PI / 180.0f));
	phobosDesc.OrbitOrientationUp = XMVector4Transform(phobosDesc.OrbitOrientationUp, Matrix::CreateFromAxisAngle(Vector3::Right, 10.0f * XM_PI / 180.0f));
	celestialBodies.insert( {"phobos", new CelestialBodyComponent(this, phobosDesc)} );

	// Deimos
	CelestialBodyDesc deimosDesc = {};
	deimosDesc.SpinSpeed = 0.8f;
	deimosDesc.Radius = 0.1f;
	deimosDesc.OrbitAnchor = celestialBodies["mars"];
	deimosDesc.OrbitRadius = 1.2f;
	deimosDesc.OrbitSpeed = 1.7f;
	deimosDesc.OrbitOrientationForward = XMVector4Transform(deimosDesc.OrbitOrientationForward, Matrix::CreateFromAxisAngle(Vector3::Right, - 5.0f * XM_PI / 180.0f));
	deimosDesc.OrbitOrientationUp = XMVector4Transform(deimosDesc.OrbitOrientationUp, Matrix::CreateFromAxisAngle(Vector3::Right, - 5.0f * XM_PI / 180.0f));
	celestialBodies.insert( {"deimos", new CelestialBodyComponent(this, deimosDesc)} );

	// Jupiter
	CelestialBodyDesc jupiterDesc = {};
	jupiterDesc.SpinSpeed = -0.8f;
	jupiterDesc.Radius = 2.0f;
	jupiterDesc.OrbitAnchor = celestialBodies["sun"];
	jupiterDesc.OrbitRadius = 50.2f;
	jupiterDesc.OrbitSpeed = 0.4f;
	jupiterDesc.OrbitOrientationForward = XMVector4Transform(jupiterDesc.OrbitOrientationForward, Matrix::CreateFromAxisAngle(Vector3::Right, 1.3f * XM_PI / 180.0f));
	jupiterDesc.OrbitOrientationUp = XMVector4Transform(jupiterDesc.OrbitOrientationUp, Matrix::CreateFromAxisAngle(Vector3::Right, 1.3f * XM_PI / 180.0f));
	jupiterDesc.PrimaryColor = Vector4(153.f / 255.f, 76.f / 255.f, 0.0f / 255.f, 1.f);
	jupiterDesc.SecondaryColor = Vector4(0.8f, 0.8f, 0.8f, 1.f);
	celestialBodies.insert( {"jupiter", new CelestialBodyComponent(this, jupiterDesc)} );

	// Adding all bodies to Components vector
	for (const std::pair<const std::string, CelestialBodyComponent*>& n : celestialBodies)
		Components.push_back(n.second);

	// Camera controller binding
	InputDevice->MouseMove.AddRaw(&controller, &CameraController::OnMouseMove);
}

void SolarGame::Update()
{
	controller.Update();
	Game::Update();
}
