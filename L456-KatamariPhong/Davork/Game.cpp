#include "Game.h"

#include "ResourceFactory.h"

LRESULT CALLBACK Game::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	Game* pThis;

	if (umessage == WM_NCCREATE)
	{
		pThis = static_cast<Game*>(reinterpret_cast<CREATESTRUCT*>(lparam)->lpCreateParams);  // NOLINT(performance-no-int-to-ptr)

		SetLastError(0);
		if (!SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis)))
		{
			if (GetLastError() != 0)
				return FALSE;
		}
	}
	else
	{
		pThis = reinterpret_cast<Game*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));  // NOLINT(performance-no-int-to-ptr)
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
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));  // NOLINT(performance-no-int-to-ptr)
			const auto lpb = new BYTE[dwSize];
			if (lpb == nullptr) {
				return 0;
			}
			
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)  // NOLINT(performance-no-int-to-ptr)
				OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

			const auto* raw = reinterpret_cast<RAWINPUT*>(lpb);

			if (pThis)
			{
				if (raw->header.dwType == RIM_TYPEKEYBOARD)
				{
					pThis->inputDevice_->OnKeyDown({
						raw->data.keyboard.MakeCode,
						raw->data.keyboard.Flags,
						raw->data.keyboard.VKey,
						raw->data.keyboard.Message
					});
				}
				else if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					pThis->inputDevice_->OnMouseMove({
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
	
	depthStencilDesc.Width = display_->ClientWidth;
	depthStencilDesc.Height = display_->ClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags = 0;

	HRESULT res = device_->CreateTexture2D(&depthStencilDesc, nullptr, depthStencilBuffer_.GetAddressOf());

	if (FAILED(res))
	{
		OutputDebugString(TEXT("Fatal error: Failed to create depth stencil buffer!\n"));
	}
	
	res = device_->CreateDepthStencilView(depthStencilBuffer_.Get(), nullptr, depthStencilView_.GetAddressOf());

	if (FAILED(res))
	{
		OutputDebugString(TEXT("Fatal error: Failed to create depth stencil view!\n"));
	}
}

void Game::CreateCsmDepthTextureArray()
{
	D3D11_TEXTURE2D_DESC depthDescription = {};
	depthDescription.Width = 1024;
	depthDescription.Height = 1024;
	depthDescription.ArraySize = 4;
	depthDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthDescription.Format = DXGI_FORMAT_R32_TYPELESS;

	auto res = device_->CreateTexture2D(&depthDescription, nullptr, shadowTexArr_.GetAddressOf());

	if (FAILED(res))
	{
		OutputDebugString(TEXT("Fatal error: Failed to create CSM depth texture array!\n"));
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dViewDesc = {};
	dViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dViewDesc.Texture2DArray = {};
	dViewDesc.Texture2DArray.MipSlice = 0;
	dViewDesc.Texture2DArray.FirstArraySlice = 0;
	dViewDesc.Texture2DArray.ArraySize = 4;

	res = device_->CreateDepthStencilView(shadowTexArr_.Get(), &dViewDesc, depthShadowDsv_.GetAddressOf());

	if (FAILED(res))
	{
		OutputDebugString(TEXT("Fatal error: Failed to create CSM depth stencil view!\n"));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray = {};
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 0;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = 4;

	res = device_->CreateShaderResourceView(shadowTexArr_.Get(), &srvDesc, depthShadowSrv_.GetAddressOf());
	
	if (FAILED(res))
	{
		OutputDebugString(TEXT("Fatal error: Failed to create CSM depth SRV!\n"));
	}
}

void Game::CreateBackBuffer()
{
	auto res = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer_.GetAddressOf()));

	if (FAILED(res))
	{
		OutputDebugString(TEXT("Fatal error: Failed to create back buffer!\n"));
	}
	
	res = device_->CreateRenderTargetView(backBuffer_.Get(), nullptr, renderView_.GetAddressOf());

	if (FAILED(res))
	{
		OutputDebugString(TEXT("Fatal error: Failed to create RTV!\n"));
	}
}

Game::Game(LPCWSTR name, int screenWidth, int screenHeight) : isExitRequested_(false), name_(name), frameCount_(0)
{
	instance_ = GetModuleHandle(nullptr);

	display_ = std::make_shared<DisplayWin32>(name, instance_, screenWidth, screenHeight, this);
	inputDevice_ = std::make_shared<InputDevice>(this);

	camera_ = std::make_shared<Camera>();
	camera_->AspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
}

Game::~Game()  // NOLINT(modernize-use-equals-default)
{
	for (const auto c : components_)
	{
		c->~GameComponent();
	}
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
		isExitRequested_ = true;
}

void Game::RestoreTargets()
{
}

void Game::InitTimer()
{
	prevTime_ = std::chrono::steady_clock::now();
}

void Game::UpdateTimer()
{
	const auto curTime = std::chrono::steady_clock::now();
	deltaTime_ = std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime_).count() / 1000000.0f;
	prevTime_ = curTime;

	totalTime_ += deltaTime_;
	frameCount_++;

	if (totalTime_ > 1.0f)
	{
		const float fps = static_cast<float>(frameCount_) / totalTime_;

		totalTime_ -= 1.0f;

		WCHAR text[256];
		swprintf_s(text, TEXT("FPS: %f"), fps);
		SetWindowText(display_->hWnd, text);

		frameCount_ = 0;
	}
}

void Game::Run()
{
	PrepareResources();
	Initialize();
	InitTimer();
	while (!isExitRequested_)
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

ID3D11Device* Game::GetDevice() const
{
	return device_.Get();
}

ID3D11DeviceContext* Game::GetContext() const
{
	return context_.Get();
}

InputDevice* Game::GetInputDevice() const
{
	return inputDevice_.get();
}

Camera* Game::GetCamera() const
{
	return camera_.get();
}

DisplayWin32* Game::GetDisplay() const
{
	return display_.get();
}

void Game::DestroyResources()
{
	for (const auto c : components_)
	{
		c->DestroyResources();
	}
	ResourceFactory::DestroyResources();
}

void Game::Draw()
{
	for (const auto c : components_)
	{
		c->Draw();
	}
}

void Game::EndFrame()
{
	context_->OMSetRenderTargets(0, nullptr, nullptr);

	swapChain_->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

void Game::Initialize()
{
	for (const auto c : components_)
	{
		c->Initialize();
	}
}

void Game::PrepareFrame()
{
	context_->ClearState();

	context_->OMSetRenderTargets(1, renderView_.GetAddressOf(), depthStencilView_.Get());

	constexpr float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context_->ClearRenderTargetView(renderView_.Get(), color);
	context_->ClearDepthStencilView(depthStencilView_.Get(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Game::PrepareResources()
{
	constexpr D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	DXGI_SWAP_CHAIN_DESC swapDesc;
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = display_->ClientWidth;
	swapDesc.BufferDesc.Height = display_->ClientHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = display_->hWnd;
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
		swapChain_.GetAddressOf(),
		device_.GetAddressOf(),
		nullptr,
		context_.GetAddressOf());

	if (FAILED(res))
	{
		OutputDebugString(TEXT("Fatal error: Failed to create device and/or swap chain!\n"));
	}

	CreateBackBuffer();

	CreateDepthStencilBuffer();

	CreateCsmDepthTextureArray();

	ResourceFactory::Initialize(this);
}

void Game::Update()
{
	camera_->UpdateMatrix();
	for (const auto c : components_)
	{
		c->Update();
	}
}

void Game::UpdateInternal()
{
}
