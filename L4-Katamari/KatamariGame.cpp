#include "KatamariGame.h"

#include "Davork/GameComponents/DebugAxisAnchorComponent.h"
#include "Davork/GameComponents/GridComponent.h"
#include "Davork/GameComponents/QuadComponent.h"
#include "Davork/GameComponents/SphereComponent.h"

using namespace DirectX;
using namespace SimpleMath;

KatamariGame::KatamariGame() : Game(L"Katamari Game", 800, 800)
{
    //srand(static_cast<unsigned>(time(nullptr)));

    //SphereComponent* sphere = new SphereComponent(this, 1.0f, 16, 16);

    QuadComponent* quad = new QuadComponent(this, L"Textures/wood.dds");

    //quad->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Right, XM_PI / 2.0f));
    //quad->SetScale(Vector3::One * 100.0f);
    //Components.push_back(quad);

    GridComponent* grid = new GridComponent(this, 1.0f, 100);
    grid->SetPosition(Vector3(0.0f, -0.001f, 0.0f));
    Components.push_back(grid);

    DebugAxisAnchorComponent* debugAnchor = new DebugAxisAnchorComponent(this);
    Components.push_back(debugAnchor);

    fpsCameraController = new FPSCameraController(this, Camera);
    
    // Camera controller binding
    InputDevice->MouseMove.AddRaw(fpsCameraController, &FPSCameraController::OnMouseMove);
}

void KatamariGame::Update()
{
    fpsCameraController->Update();
    Game::Update();
}
