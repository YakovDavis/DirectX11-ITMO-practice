#include "KatamariGame.h"

using namespace DirectX;
using namespace SimpleMath;

KatamariGame::KatamariGame() : Game(L"Katamari Game", 800, 800)
{
    //srand(static_cast<unsigned>(time(nullptr)));
    // Camera controller binding
    InputDevice->MouseMove.AddRaw(&controller, &CameraController::OnMouseMove);
}

void KatamariGame::Update()
{
    controller.Update();
    Game::Update();
}
