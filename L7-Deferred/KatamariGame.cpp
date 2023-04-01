#include "KatamariGame.h"

#include "Davork/GameComponents/DebugAxisAnchorComponent.h"
#include "Davork/GameComponents/GridComponent.h"
#include "GameComponents/KatamariFurnitureComponent.h"
#include "Davork/GameComponents/QuadComponent.h"
#include "GameComponents/KatamariBall.h"

using namespace DirectX;
using namespace SimpleMath;

KatamariGame::KatamariGame() : Game(L"Katamari Game", 800, 800)
{
    srand(static_cast<unsigned>(time(nullptr)));

    ball = new KatamariBall(this);
    ball->SetPosition(Vector3(0.0f, 1.0f, 0.0f));
    components_.push_back(ball);

    QuadComponent* quad = new QuadComponent(this, L"Textures/wood.dds");
    quad->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Right, XM_PI / 2.0f));
    quad->SetScale(Vector3::One * 100.0f);
    components_.push_back(quad);

    for (int i = 0; i < 10; ++i)
    {
        KatamariFurnitureComponent* stool = new KatamariFurnitureComponent(this, "Models/stool.obj", L"Textures/stool.dds", 1.5f, Vector3(0.0f, 0.8f, 0.0f));
        stool->SetPosition(Vector3(static_cast<float>(rand()) / RAND_MAX * 100.0f - 50.0f, 0.0f, static_cast<float>(rand()) / RAND_MAX  * 100.0f - 50.0f));
        stool->collision.Radius = 0.8f;
        components_.push_back(stool);
        furniture.push_back(stool);
    }

    for (int i = 0; i < 10; ++i)
    {
        KatamariFurnitureComponent* strawberry = new KatamariFurnitureComponent(this, "Models/strawberry.obj", L"Textures/strawberry.dds", 0.4f, Vector3(0.0f, 0.5f, 0.0f));
        strawberry->SetScale(Vector3(0.1f, 0.1f, 0.1f));
        strawberry->SetPosition(Vector3(static_cast<float>(rand()) / RAND_MAX * 100.0f - 50.0f, 0.0f, static_cast<float>(rand()) / RAND_MAX  * 100.0f - 50.0f));
        strawberry->collision.Radius = 0.1f;
        components_.push_back(strawberry);
        furniture.push_back(strawberry);
    }

    for (int i = 0; i < 10; ++i)
    {
        KatamariFurnitureComponent* duck = new KatamariFurnitureComponent(this, "Models/duck.obj", L"Textures/duck.dds", 1.2f, Vector3(0.0f, 1.0f, 0.0f));
        duck->SetScale(Vector3(0.02f, 0.02f, 0.02f));
        duck->SetPosition(Vector3(static_cast<float>(rand()) / RAND_MAX * 100.0f - 50.0f, 0.0f, static_cast<float>(rand()) / RAND_MAX  * 100.0f - 50.0f));
        duck->collision.Radius = 0.2f;
        components_.push_back(duck);
        furniture.push_back(duck);
    }

    KatamariFurnitureComponent* sofa = new KatamariFurnitureComponent(this, "Models/sofa.obj", L"Textures/sofa1.dds", 3.0f, Vector3(0.0f, 1.0f, 0.0f));
    sofa->SetScale(Vector3(7.0f, 7.0f, 7.0f));
    sofa->SetPosition(Vector3(-1.0f, 0.0f, 10.0f));
    sofa->collision.Radius = 2.0f;
    components_.push_back(sofa);
    furniture.push_back(sofa);

    KatamariFurnitureComponent* sofa1 = new KatamariFurnitureComponent(this, "Models/sofa.obj", L"Textures/sofa1.dds", 3.0f, Vector3(0.0f, 1.0f, 0.0f));
    sofa1->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Up, XM_PI / 2.0f));
    sofa1->SetScale(Vector3(7.0f, 7.0f, 7.0f));
    sofa1->SetPosition(Vector3(-8.0f, 0.0f, 10.0f));
    sofa->collision.Radius = 2.0f;
    components_.push_back(sofa1);
    furniture.push_back(sofa1);

    /*GridComponent* grid = new GridComponent(this, 1.0f, 100);
    grid->SetPosition(Vector3(0.0f, -0.001f, 0.0f));
    Components.push_back(grid);

    DebugAxisAnchorComponent* debugAnchor = new DebugAxisAnchorComponent(this);
    Components.push_back(debugAnchor);*/

    orbitCameraController = new OrbitCameraController(this, GetCamera(), ball);
    orbitCameraController->isLMBActivated = true;
    
    // Camera controller binding
    inputDevice_->MouseMove.AddRaw(orbitCameraController, &OrbitCameraController::OnMouseMove);
}

void KatamariGame::Update()
{
    orbitCameraController->Update();
    Vector3 dir = Vector3::Zero;
    if (inputDevice_->IsKeyDown(Keys::W))
        dir += orbitCameraController->GetForward();
    if (inputDevice_->IsKeyDown(Keys::S))
        dir -= orbitCameraController->GetForward();
    if (inputDevice_->IsKeyDown(Keys::A))
        dir -= (orbitCameraController->GetForward()).Cross(orbitCameraController->GetUp());
    if (inputDevice_->IsKeyDown(Keys::D))
        dir +=(orbitCameraController->GetForward()).Cross(orbitCameraController->GetUp());
    if (dir.Length() > 0.0f)
        ball->SetDirection(dir);
    Game::Update();
}
