#include "KatamariGame.h"

#include "Davork/GameComponents/DebugAxisAnchorComponent.h"
#include "Davork/GameComponents/GridComponent.h"
#include "Davork/GameComponents/QuadComponent.h"
#include "GameComponents/KatamariBall.h"

using namespace DirectX;
using namespace SimpleMath;

KatamariGame::KatamariGame() : Game(L"Katamari Game", 800, 800)
{
    //srand(static_cast<unsigned>(time(nullptr)));

    ball = new KatamariBall(this);
    ball->SetPosition(Vector3(0.0f, 1.0f, 0.0f));
    Components.push_back(ball);

    QuadComponent* quad = new QuadComponent(this, L"Textures/wood.dds");
    quad->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Right, XM_PI / 2.0f));
    quad->SetScale(Vector3::One * 1000.0f);
    Components.push_back(quad);

    /*GridComponent* grid = new GridComponent(this, 1.0f, 100);
    grid->SetPosition(Vector3(0.0f, -0.001f, 0.0f));
    Components.push_back(grid);

    DebugAxisAnchorComponent* debugAnchor = new DebugAxisAnchorComponent(this);
    Components.push_back(debugAnchor);*/

    orbitCameraController = new OrbitCameraController(this, Camera, ball);
    
    // Camera controller binding
    InputDevice->MouseMove.AddRaw(orbitCameraController, &OrbitCameraController::OnMouseMove);
}

void KatamariGame::Update()
{
    orbitCameraController->Update();
    if (InputDevice->IsKeyDown(Keys::W))
        ball->SetDirection(orbitCameraController->GetForward());
    Game::Update();
}
