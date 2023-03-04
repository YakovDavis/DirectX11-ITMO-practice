#pragma once
#include "SphereComponent.h"

class CelestialBodyComponent;

struct CelestialBodyDesc
{
    float Radius;
    float SpinSpeed;
    CelestialBodyComponent* OrbitAnchor;
    DirectX::SimpleMath::Vector3 OrbitOrientation1;
    DirectX::SimpleMath::Vector3 OrbitOrientation2;
    float OrbitRadius;
    float OrbitSpeed;
    DirectX::SimpleMath::Vector4 PrimaryColor;
    DirectX::SimpleMath::Vector4 SecondaryColor;
    CelestialBodyDesc() : Radius(1.0f), SpinSpeed(1.0f), OrbitAnchor(nullptr),
        OrbitOrientation1(DirectX::SimpleMath::Vector3::Forward),
        OrbitOrientation2(DirectX::SimpleMath::Vector3::Right),
        OrbitRadius(0.0f), OrbitSpeed(0.0f), PrimaryColor(DirectX::SimpleMath::Vector4::One),
        SecondaryColor(DirectX::SimpleMath::Vector4::Zero) {}
};

class CelestialBodyComponent : public SphereComponent
{
public:
    CelestialBodyComponent(Game* game, const CelestialBodyDesc& desc);
    void Update() override;
};
