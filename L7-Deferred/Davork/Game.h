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
#include "PerSceneCb.h"
#include "DirectionalLight.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class Game
{
	friend class DisplayWin32;
	
	void CreateBackBuffer();
	void CreateDepthStencilBuffer();
	void CreateCsmDepthTextureArray();
	void InitTimer();
	void UpdateTimer();
	
protected:
	bool isExitRequested_;
	virtual void DestroyResources();
	virtual void Draw();
	virtual void EndFrame();
	virtual void Initialize();
	virtual void PrepareFrame();
	virtual void PrepareResources();
	virtual void Update();
	virtual void UpdateInternal();
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
	int debugAnnotation_{};
	Microsoft::WRL::ComPtr<ID3D11Device> device_;
	HINSTANCE instance_;
	LPCWSTR name_;
	std::chrono::steady_clock::time_point prevTime_{};
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderSrv_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderView_;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView_;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexArr_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthShadowDsv_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthShadowSrv_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> perSceneCBuffer_;
	PerSceneCb sceneData_{};
	DirectionalLight dLight_;
	bool screenResized_{};
	float startTime_{};
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
	float totalTime_{};
	float deltaTime_{};
	std::vector<GameComponent*> components_{};
	std::shared_ptr<DisplayWin32> display_;
	std::shared_ptr<InputDevice> inputDevice_;
	std::shared_ptr<Camera> camera_;
	unsigned int frameCount_;
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
	
public:
	Game(LPCWSTR name, int screenWidth, int screenHeight);
	virtual ~Game();
	void Exit();
	void MessageHandler();
	void RestoreTargets();
	void Run();
	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetContext() const;
	ID3D11ShaderResourceView* GetCsm() const;
	ID3D11Buffer* const* GetPerSceneCb() const;
	DirectionalLight* GetDLight();
	InputDevice* GetInputDevice() const;
	Camera* GetCamera() const;
	DisplayWin32* GetDisplay() const;
	float DeltaTime() const { return deltaTime_; }
	
};
