#include "CelestialBodyComponent.h"

constexpr int SliceCount = 32;
constexpr int StackCount = 32;

CelestialBodyComponent::CelestialBodyComponent(Game* game, const CelestialBodyDesc& desc) :
    SphereComponent(game, desc.Radius, SliceCount, StackCount, desc.PrimaryColor, desc.SecondaryColor)
{
    
}

void CelestialBodyComponent::Update()
{
    SphereComponent::Update();
}
