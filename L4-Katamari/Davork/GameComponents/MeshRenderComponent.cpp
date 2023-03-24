#include "MeshRenderComponent.h"
#include <iostream>
#include "../ResourceFactory.h"

using namespace DirectX;
using namespace SimpleMath;

MeshRenderComponent::MeshRenderComponent(Game* g, const std::string fileNameModel, const wchar_t* fileNameTexture) :
    BaseRenderComponent(g), fNameModel(fileNameModel), isPickedUp(false)
{
    textureFileName = fileNameTexture;
}

void MeshRenderComponent::Initialize()
{
    points = ResourceFactory::GetPoints(fNameModel);
    indices = ResourceFactory::GetIndices(fNameModel);
    
    BaseRenderComponent::Initialize();
}
