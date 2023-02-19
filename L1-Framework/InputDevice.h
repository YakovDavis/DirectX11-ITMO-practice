#pragma once
#include <set>

class Game;

class InputDevice
{
private:
	Game* game;
	std::set<unsigned int> keys;
protected:
	void OnKeyDown();
	void OnMouseMove();
public:
	InputDevice(Game* g);
	~InputDevice();
	int MousePosX;
	int MousePosY;
	void AddPressedKey(unsigned int key);
	bool IsKeyDown(unsigned int key);
	void RemovePressed(unsigned int key);
	void OnRawDelta(int x, int y);
};
