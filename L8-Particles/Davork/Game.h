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
#include "GBuffer.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class LightVolumeComponent;

class Game
{
	friend class DisplayWin32;

public:
	struct CbDataCascade
	{
		DirectX::SimpleMath::Matrix ViewProj[5];
		DirectX::SimpleMath::Vector4 Distance;
	};
	
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
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> defaultDepthState_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> quadDepthState_;
	int debugAnnotation_{};
	Microsoft::WRL::ComPtr<ID3D11Device> device_;
	HINSTANCE instance_;
	LPCWSTR name_;
	std::chrono::steady_clock::time_point prevTime_{};
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderSrv_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderView_;
	GBuffer gBuffer_;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView_;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexArr_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthShadowDsv_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthShadowSrv_;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> depthSamplerState_;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> perSceneCBuffer_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cascadeCBuffer_;
	PerSceneCb sceneData_{};
	DirectionalLight dLight_;
	std::shared_ptr<LightVolumeComponent> lightVolumeComponent_;
	bool screenResized_{};
	float startTime_{};
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
	float totalTime_{};
	float deltaTime_{};
	std::vector<GameComponent*> components_{};
	std::vector<GameComponent*> transparentComponents_{};
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
	ID3D11Buffer* const* GetCascadeCb() const;
	DirectionalLight* GetDLight();
	InputDevice* GetInputDevice() const;
	Camera* GetCamera() const;
	DisplayWin32* GetDisplay() const;
	ID3D11RenderTargetView** GetMainRTV();
	ID3D11DepthStencilView* GetMainDSV();
	float DeltaTime() const { return deltaTime_; }
	
};
