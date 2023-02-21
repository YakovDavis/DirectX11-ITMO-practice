#pragma once
#include <set>
#include "Keyboard.h"

class Game;

class InputDevice
{
private:
	Game* game;
	std::unique_ptr<DirectX::Keyboard> keyboard;
public:
	InputDevice(Game* g);
	~InputDevice();
	int MousePosX;
	int MousePosY;
	bool IsKeyDown(DirectX::Keyboard::Keys key);
	bool IsKeyUp(DirectX::Keyboard::Keys key);
	void OnRawDelta(int x, int y);
};
