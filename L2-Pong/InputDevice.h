#pragma once

class Game;

class InputDevice
{
private:
	Game* game;
	int keys;
protected:
	void OnKeyDown();
	void OnMouseMove();
public:
	int MouseMove;
	int MouseOffset;
	int MousePosition;
	int MouseWheelID;
	void AddPressedKey();
	void IsKeyDown();
	void RemovePressed();
};
