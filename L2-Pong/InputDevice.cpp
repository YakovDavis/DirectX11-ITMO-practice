#include "InputDevice.h"
#include "Game.h"

using namespace DirectX;

InputDevice::InputDevice(Game* g) : game(g)
{
    keyboard = std::make_unique<Keyboard>();
    MousePosX = 0;
    MousePosY = 0;
}

InputDevice::~InputDevice()
{
}

bool InputDevice::IsKeyDown(Keyboard::Keys key)
{
    auto kb = keyboard->GetState();
    return kb.IsKeyDown(key);
}

bool InputDevice::IsKeyUp(Keyboard::Keys key)
{
    auto kb = keyboard->GetState();
    return kb.IsKeyUp(key);
}

void InputDevice::OnRawDelta(int x, int y)
{
    MousePosX += x;
    MousePosY += y;
}
