#include "Game.h"

void Game::CreateBackBuffer()
{
	auto res = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);	// __uuidof(ID3D11Texture2D)
	res = Device->CreateRenderTargetView(backBuffer, nullptr, &RenderView);
}

Game::Game(LPCWSTR name, int screenWidth, int screenHeight) : Name(name), FrameCount(0), Components()
{
	Instance = GetModuleHandle(nullptr);

	Display = new DisplayWin32(name, Instance, screenWidth, screenHeight);

	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = Display->ClientWidth;
	swapDesc.BufferDesc.Height = Display->ClientHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = Display->hWnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	auto res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		&SwapChain,
		&Device,
		nullptr,
		&Context);

	if (FAILED(res))
	{
		// Well, that was unexpected
	}

	CreateBackBuffer();
}

Game::~Game()
{
	delete Display;
	delete InputDevice;
	Context->Release();
	backBuffer->Release();
	RenderView->Release();
	SwapChain->Release();
}

void Game::Exit()
{
	DestroyResources();
}

void Game::MessageHandler(MSG& msg)
{
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Game::RestoreTargets()
{
}

void Game::Run()
{
	Initialize();
	MSG msg = {};
	bool isExitRequested = false;
	PrevTime = std::chrono::steady_clock::now();
	while (!isExitRequested) {
		// Handle the windows messages.
		MessageHandler(msg);

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT) {
			isExitRequested = true;
		}

		auto	curTime = std::chrono::steady_clock::now();
		float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
		PrevTime = curTime;

		TotalTime += deltaTime;
		FrameCount++;

		if (TotalTime > 1.0f) {
			float fps = FrameCount / TotalTime;

			TotalTime -= 1.0f;

			WCHAR text[256];
			swprintf_s(text, TEXT("FPS: %f"), fps);
			SetWindowText(Display->hWnd, text);

			FrameCount = 0;
		}

		Context->ClearState();

		Context->OMSetRenderTargets(1, &RenderView, nullptr);

		Draw();

		Context->OMSetRenderTargets(0, nullptr, nullptr);

		SwapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
	}
	Exit();
}
