#pragma once
#include <windows.h>
#include <WinUser.h>
#include <iostream>

class DisplayWin32
{
public:
	int ClientHeight;
	int ClientWidth;
	HINSTANCE hInstance;
	HWND hWnd;
	int Module; // ?
	WNDCLASSEX wc;
	DisplayWin32(LPCWSTR applicationName, HINSTANCE hInst, int screenWidth, int screenHeight);
	void SetText(WCHAR text[]);
};
