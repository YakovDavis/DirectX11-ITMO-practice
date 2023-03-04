#include "Game.h"

LRESULT CALLBACK Game::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	Game* pThis;

	if (umessage == WM_NCCREATE)
	{
		pThis = static_cast<Game*>(reinterpret_cast<CREATESTRUCT*>(lparam)->lpCreateParams);

		SetLastError(0);
		if (!SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis)))
		{
			if (GetLastError() != 0)
				return FALSE;
		}
	}
	else
	{
		pThis = reinterpret_cast<Game*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	
	switch (umessage)
	{
	case WM_KEYDOWN:
		{
			if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
			return 0;
		}
	case WM_INPUT:
		{
			UINT dwSize = 0;
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
			LPBYTE lpb = new BYTE[dwSize];
			if (lpb == nullptr) {
				return 0;
			}
			
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
				OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

			auto* raw = reinterpret_cast<RAWINPUT*>(lpb);

			if (pThis)
			{
				if (raw->header.dwType == RIM_TYPEKEYBOARD)
				{
					//printf(" Kbd: make=%04i Flags:%04i Reserved:%04i ExtraInformation:%08i, msg=%04i VK=%i \n",
					//	raw->data.keyboard.MakeCode,
					//	raw->data.keyboard.Flags,
					//	raw->data.keyboard.Reserved,
					//	raw->data.keyboard.ExtraInformation,
					//	raw->data.keyboard.Message,
					//	raw->data.keyboard.VKey);

					pThis->InputDev->OnKeyDown({
						raw->data.keyboard.MakeCode,
						raw->data.keyboard.Flags,
						raw->data.keyboard.VKey,
						raw->data.keyboard.Message
					});
				}
				else if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					//printf(" Mouse: X=%04d Y:%04d \n", raw->data.mouse.lLastX, raw->data.mouse.lLastY);
					pThis->InputDev->OnMouseMove({
						raw->data.mouse.usFlags,
						raw->data.mouse.usButtonFlags,
						static_cast<int>(raw->data.mouse.ulExtraInformation),
						static_cast<int>(raw->data.mouse.ulRawButtons),
						static_cast<short>(raw->data.mouse.usButtonData),
						raw->data.mouse.lLastX,
						raw->data.mouse.lLastY
					});
				}
			}
			
			delete[] lpb;
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
	default:
		{
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
	}
}

void Game::CreateDepthStencilBuffer()
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	
	depthStencilDesc.Width     = Display->ClientWidth;
	depthStencilDesc.Height    = Display->ClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	HRESULT res = Device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
	
	res = Device->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencilView);
}

void Game::CreateBackBuffer()
{
	auto res = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	res = Device->CreateRenderTargetView(backBuffer, nullptr, &RenderView);
}

Game::Game(LPCWSTR name, int screenWidth, int screenHeight) : isExitRequested(false), Name(name), FrameCount(0)
{
	Instance = GetModuleHandle(nullptr);

	Display = new DisplayWin32(name, Instance, screenWidth, screenHeight, this);
	InputDev = new InputDevice(this);

	Camera = new ::Camera();
	Camera->AspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
}

Game::~Game()
{
	for (auto c : Components)
	{
		delete c;
	}
	delete Display;
	delete InputDev;
}

void Game::Exit()
{
	DestroyResources();
}

void Game::MessageHandler()
{
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT)
		isExitRequested = true;
}

void Game::RestoreTargets()
{
}

void Game::InitTimer()
{
	PrevTime = std::chrono::steady_clock::now();
}

void Game::UpdateTimer()
{
	auto	curTime = std::chrono::steady_clock::now();
	DeltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
	PrevTime = curTime;

	TotalTime += DeltaTime;
	FrameCount++;

	if (TotalTime > 1.0f)
	{
		float fps = static_cast<float>(FrameCount) / TotalTime;

		TotalTime -= 1.0f;

		WCHAR text[256];
		swprintf_s(text, TEXT("FPS: %f"), fps);
		SetWindowText(Display->hWnd, text);

		FrameCount = 0;
	}
}

void Game::Run()
{
	PrepareResources();
	Initialize();
	InitTimer();
	while (!isExitRequested)
	{
		MessageHandler();

		UpdateTimer();

		UpdateInternal();

		Update();

		PrepareFrame();
		
		Draw();

		EndFrame();
	}
	Exit();
}

void Game::DestroyResources()
{
	for (auto c : Components)
	{
		c->DestroyResources();
	}
	
	Context->Release();
	backBuffer->Release();
	RenderView->Release();
	SwapChain->Release();
	depthStencilView->Release();
	depthStencilBuffer->Release();
}

void Game::Draw()
{
	for (auto c : Components)
	{
		c->Draw();
	}
}

void Game::EndFrame()
{
	Context->OMSetRenderTargets(0, nullptr, nullptr);

	SwapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

void Game::Initialize()
{
	for (auto c : Components)
	{
		c->Initialize();
	}
}

void Game::PrepareFrame()
{
	Context->ClearState();

	Context->OMSetRenderTargets(1, &RenderView, depthStencilView);

	constexpr float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	Context->ClearRenderTargetView(RenderView, color);
	Context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Game::PrepareResources()
{
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
		D3D11_CREATE_DEVICE_DEBUG|D3D11_CREATE_DEVICE_BGRA_SUPPORT,
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

	CreateDepthStencilBuffer();
}

void Game::Update()
{
	Camera->UpdateMatrix();
	for (auto c : Components)
	{
		c->Update();
	}
}

void Game::UpdateInternal()
{
}
