#pragma once

#include <vector>
#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <directxmath.h>
#include <chrono>
#include "Camera.h"
#include "GameComponent.h"
#include "DisplayWin32.h"
#include "InputDevice.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class Game
{
	void CreateBackBuffer();
	void CreateDepthStencilBuffer();
	void InitTimer();
	void UpdateTimer();
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
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilView* depthStencilView;
	bool ScreenResized;
	float StartTime;
	IDXGISwapChain* SwapChain;
	float TotalTime;
	float DeltaTime;
	std::vector<GameComponent*> Components;
	DisplayWin32* Display;
	InputDevice* InputDevice;
	Camera* Camera;
	unsigned int FrameCount;
	Game(LPCWSTR name, int screenWidth, int screenHeight);
	virtual ~Game();
	void Exit();
	void MessageHandler();
	void RestoreTargets();
	void Run();
};