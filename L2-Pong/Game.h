#pragma once
#include <vector>
#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <chrono>
#include "GameComponent.h"
#include "DisplayWin32.h"
#include "InputDevice.h"

class Game
{
private:
	void CreateBackBuffer();
protected:
	bool isExitRequested;
	virtual void DestroyResources();
	virtual void Draw();
	virtual void EndFrame();
	virtual void Initialize();
	virtual void PrepareFrame();
	virtual void PrepareResources();
	virtual void Update();
	virtual void UpdateInternal();
public:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
	ID3D11Texture2D* backBuffer;
	ID3D11DeviceContext* Context;
	int DebugAnnotation;
	Microsoft::WRL::ComPtr<ID3D11Device> Device;
	HINSTANCE Instance;
	LPCWSTR Name;
	std::chrono::steady_clock::time_point PrevTime;
	ID3D11Texture2D* RenderSRV;
	ID3D11RenderTargetView* RenderView;
	int ScreenResized;
	float StartTime;
	IDXGISwapChain* SwapChain;
	float TotalTime;
	float DeltaTime;
	std::vector<GameComponent*> Components;
	DisplayWin32* Display;
	InputDevice* InputDev;
	unsigned int FrameCount;
	Game(LPCWSTR name, int screenWidth, int screenHeight);
	virtual ~Game();
	void Exit();
	void MessageHandler();
	void RestoreTargets();
	void Run();
};