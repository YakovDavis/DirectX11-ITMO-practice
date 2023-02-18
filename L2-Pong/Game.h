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
	virtual void DestroyResources() = 0;
	virtual void Draw() = 0;
	virtual void EndFrame() = 0;
	virtual void Initialize() = 0;
	virtual void PrepareFrame() = 0;
	virtual void PrepareResources() = 0;
	virtual void Update() = 0;
	virtual void UpdateInternal() = 0;
public:
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
	std::vector<GameComponent*> Components;
	DisplayWin32* Display;
	InputDevice* InputDevice;
	unsigned int FrameCount;
	Game(LPCWSTR name, int screenWidth, int screenHeight);
	virtual ~Game();
	void Exit();
	void MessageHandler(MSG& msg);
	void RestoreTargets();
	void Run();
};
