#include "InputDevice.h"
#include "Game.h"

void InputDevice::OnKeyDown()
{
}

void InputDevice::OnMouseMove()
{
}

InputDevice::InputDevice(Game* g) : game(g), keys()
{
    MousePosX = 0;
    MousePosY = 0;
}

InputDevice::~InputDevice()
{
}

void InputDevice::AddPressedKey(unsigned int key)
{
    keys.insert(key);
}

bool InputDevice::IsKeyDown(unsigned int key)
{
    return keys.find(key) != keys.end();
}

void InputDevice::RemovePressed(unsigned int key)
{
    keys.erase(key);
}

void InputDevice::OnRawDelta(int x, int y)
{
    MousePosX += x;
    MousePosY += y;
}
