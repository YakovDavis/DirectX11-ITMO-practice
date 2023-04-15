#include "Game.h"

#include "ResourceFactory.h"
#include "GameComponents/LightVolumeComponent.h"

using namespace DirectX;
using namespace SimpleMath;

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
	depthDescription.Width = 2048;
	depthDescription.Height = 2048;
	depthDescription.ArraySize = 5;
	depthDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthDescription.Format = DXGI_FORMAT_R32_TYPELESS;
	depthDescription.MipLevels = 1;
	depthDescription.SampleDesc.Count = 1;
	depthDescription.SampleDesc.Quality = 0;
	depthDescription.Usage = D3D11_USAGE_DEFAULT;
	depthDescription.CPUAccessFlags = 0;
	depthDescription.MiscFlags = 0;

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
	dViewDesc.Texture2DArray.ArraySize = 5;

	res = device_->CreateDepthStencilView(shadowTexArr_.Get(), &dViewDesc, depthShadowDsv_.GetAddressOf());

	if (FAILED(res))
	{
		OutputDebugString(TEXT("Fatal error: Failed to create CSM depth stencil view!\n"));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray = {};
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = 5;

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

Game::Game(LPCWSTR name, int screenWidth, int screenHeight) : isExitRequested_(false), name_(name), frameCount_(0), dLight_(this), gBuffer_(this)
{
	instance_ = GetModuleHandle(nullptr);

	display_ = std::make_shared<DisplayWin32>(name, instance_, screenWidth, screenHeight, this);
	inputDevice_ = std::make_shared<InputDevice>(this);

	camera_ = std::make_shared<Camera>();
	camera_->AspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

	lightVolumeComponent_ = std::make_shared<LightVolumeComponent>(this);
}

Game::~Game()  // NOLINT(modernize-use-equals-default)
{
	for (const auto c : components_)
	{
		c->~GameComponent();
	}
	lightVolumeComponent_->~LightVolumeComponent();
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

ID3D11ShaderResourceView* Game::GetCsm() const
{
	return depthShadowSrv_.Get();
}

ID3D11Buffer* const* Game::GetPerSceneCb() const
{
	return perSceneCBuffer_.GetAddressOf();
}

ID3D11Buffer* const* Game::GetCascadeCb() const
{
	return cascadeCBuffer_.GetAddressOf();
}

DirectionalLight* Game::GetDLight()
{
	return &dLight_;
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
	lightVolumeComponent_->DestroyResources();
	ResourceFactory::DestroyResources();
}

void Game::Draw()
{
	context_->ClearState();
	context_->OMSetDepthStencilState(defaultDepthState_.Get(), 0);

	const auto rtvs = new ID3D11RenderTargetView*[3];
	rtvs[0] = gBuffer_.albedoRtv_.Get();
	rtvs[1] = gBuffer_.positionRtv_.Get();
	rtvs[2] = gBuffer_.normalRtv_.Get();
	context_->OMSetRenderTargets(3, rtvs, depthStencilView_.Get());

	constexpr float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context_->ClearRenderTargetView(gBuffer_.albedoRtv_.Get(), color);
	context_->ClearRenderTargetView(gBuffer_.positionRtv_.Get(), color);
	context_->ClearRenderTargetView(gBuffer_.normalRtv_.Get(), color);
	context_->ClearDepthStencilView(depthStencilView_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	for (const auto c : components_)
	{
		c->Draw();
	}

	context_->ClearState();

	context_->RSSetState(rastState_.Get());
	context_->OMSetDepthStencilState(quadDepthState_.Get(), 0);

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(display_->ClientWidth);
	viewport.Height = static_cast<float>(display_->ClientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context_->RSSetViewports(1, &viewport);
	
	context_->OMSetRenderTargets(1, renderView_.GetAddressOf(), nullptr);
	context_->OMSetBlendState(nullptr, nullptr, 0xffffffff);

	GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context_->VSSetShader(ResourceFactory::GetVertexShader("lightpass"), nullptr, 0);
	context_->PSSetShader(ResourceFactory::GetPixelShader("lightpass"), nullptr, 0);
	context_->PSSetShaderResources(0, 1, gBuffer_.albedoSrv_.GetAddressOf());
	context_->PSSetShaderResources(1, 1, gBuffer_.positionSrv_.GetAddressOf());
	context_->PSSetShaderResources(2, 1, gBuffer_.normalSrv_.GetAddressOf());
	context_->PSSetShaderResources(3, 1, depthShadowSrv_.GetAddressOf());
	context_->PSSetConstantBuffers(0, 1, perSceneCBuffer_.GetAddressOf());
	context_->PSSetConstantBuffers(1, 1, cascadeCBuffer_.GetAddressOf());
	context_->PSSetSamplers(0, 1, depthSamplerState_.GetAddressOf());

	context_->Draw(4, 0);

	context_->OMSetBlendState(blendState_.Get(), nullptr, 0xffffffff);

	sceneData_.AmbientSpecularPowType = Vector4(0.4f, 0.5f, 32, 1);
	
	sceneData_.LightPos = Vector4(3, 1, 3, 1);
	sceneData_.LightPos = Vector4::Transform(sceneData_.LightPos, GetCamera()->GetView());
	sceneData_.LightColor = Vector4(1, 0, 0, 1) * 2.0f;

	GetContext()->UpdateSubresource(perSceneCBuffer_.Get(), 0, nullptr, &sceneData_, 0, 0);
	
	lightVolumeComponent_->SetSize(10.0f);
	lightVolumeComponent_->SetPosition(Vector3(3, 1, 3));
	lightVolumeComponent_->Update();
	lightVolumeComponent_->Draw();

	sceneData_.LightPos = Vector4(-3, 1, 3, 1);
	sceneData_.LightPos = Vector4::Transform(sceneData_.LightPos, GetCamera()->GetView());
	sceneData_.LightColor = Vector4(0, 1, 0, 1) * 2.0f;

	GetContext()->UpdateSubresource(perSceneCBuffer_.Get(), 0, nullptr, &sceneData_, 0, 0);

	lightVolumeComponent_->SetSize(10.0f);
	lightVolumeComponent_->SetPosition(Vector3(-3, 1, 3));
	lightVolumeComponent_->Update();
	lightVolumeComponent_->Draw();

	sceneData_.LightPos = Vector4(3, 1, -3, 1);
	sceneData_.LightPos = Vector4::Transform(sceneData_.LightPos, GetCamera()->GetView());
	sceneData_.LightColor = Vector4(0, 0, 1, 1) * 2.0f;

	GetContext()->UpdateSubresource(perSceneCBuffer_.Get(), 0, nullptr, &sceneData_, 0, 0);

	lightVolumeComponent_->SetSize(10.0f);
	lightVolumeComponent_->SetPosition(Vector3(3, 1, -3));
	lightVolumeComponent_->Update();
	lightVolumeComponent_->Draw();
	
	sceneData_.LightPos = Vector4(-3, 1, -3, 1);
	sceneData_.LightPos = Vector4::Transform(sceneData_.LightPos, GetCamera()->GetView());
	sceneData_.LightColor = Vector4(1, 1, 1, 1) * 2.0f;
	
	GetContext()->UpdateSubresource(perSceneCBuffer_.Get(), 0, nullptr, &sceneData_, 0, 0);

	lightVolumeComponent_->SetSize(10.0f);
	lightVolumeComponent_->SetPosition(Vector3(-3, 1, -3));
	lightVolumeComponent_->Update();
	lightVolumeComponent_->Draw();
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

	context_->OMSetRenderTargets(0, nullptr, depthShadowDsv_.Get());

	context_->ClearDepthStencilView(depthShadowDsv_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	for (const auto c : components_)
	{
		c->PrepareFrame();
	}
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

	gBuffer_.Initialize();

	D3D11_BUFFER_DESC constBufPerSceneDesc;
	constBufPerSceneDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufPerSceneDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufPerSceneDesc.CPUAccessFlags = 0;
	constBufPerSceneDesc.MiscFlags = 0;
	constBufPerSceneDesc.StructureByteStride = 0;
	constBufPerSceneDesc.ByteWidth = sizeof(PerSceneCb);

	GetDevice()->CreateBuffer(&constBufPerSceneDesc, nullptr, perSceneCBuffer_.GetAddressOf());

	D3D11_BUFFER_DESC constBufCascadeDesc;
	constBufCascadeDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufCascadeDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufCascadeDesc.CPUAccessFlags = 0;
	constBufCascadeDesc.MiscFlags = 0;
	constBufCascadeDesc.StructureByteStride = 0;
	constBufCascadeDesc.ByteWidth = sizeof(Matrix) * 5 + sizeof(Vector4);

	GetDevice()->CreateBuffer(&constBufCascadeDesc, nullptr, cascadeCBuffer_.GetAddressOf());

	CD3D11_RASTERIZER_DESC rastDesc = {};
	
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.FrontCounterClockwise = true;
	rastDesc.DepthClipEnable = true;

	res = device_->CreateRasterizerState(&rastDesc, rastState_.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC defaultDepthDesc = {};

	defaultDepthDesc.DepthEnable = true;
	defaultDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	defaultDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;

	res = device_->CreateDepthStencilState(&defaultDepthDesc, defaultDepthState_.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC quadDepthDesc = {};

	defaultDepthDesc.DepthEnable = true;
	defaultDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	defaultDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;

	res = device_->CreateDepthStencilState(&quadDepthDesc, quadDepthState_.GetAddressOf());

	D3D11_SAMPLER_DESC depthSamplerStateDesc = {};
	depthSamplerStateDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	depthSamplerStateDesc.ComparisonFunc = D3D11_COMPARISON_GREATER_EQUAL;
	depthSamplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	depthSamplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	depthSamplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	depthSamplerStateDesc.BorderColor[0] = 1.0f;
	depthSamplerStateDesc.BorderColor[1] = 1.0f;
	depthSamplerStateDesc.BorderColor[2] = 1.0f;
	depthSamplerStateDesc.BorderColor[3] = 1.0f;

	res = GetDevice()->CreateSamplerState(&depthSamplerStateDesc, depthSamplerState_.GetAddressOf());

	D3D11_BLEND_DESC blendDesc = {};
	
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE; //D3D11_BLEND_SRC_COLOR;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE; //D3D11_BLEND_BLEND_FACTOR;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
	blendDesc.AlphaToCoverageEnable = false;

	res = GetDevice()->CreateBlendState(&blendDesc, blendState_.GetAddressOf());

	lightVolumeComponent_->Initialize();
}

void Game::Update()
{
	auto tmp = Vector4(20.0f, 50.0f, 20.0f, 0.0f);
	tmp.Normalize();
	dLight_.SetDirection(tmp);
	sceneData_.LightPos = Vector4::Transform(dLight_.GetDirection(), GetCamera()->GetView());
	sceneData_.LightPos.Normalize();
	sceneData_.LightColor = dLight_.GetColor();
	sceneData_.AmbientSpecularPowType = Vector4(0.4f, 0.5f, 32, 0);
	sceneData_.T = Matrix(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	sceneData_.ViewMatrix = GetCamera()->GetView();

	CbDataCascade cascadeData = {};
	auto tmp1 = GetDLight()->GetLightSpaceMatrices();
	for (int i = 0; i < 5; ++i)
	{
		cascadeData.ViewProj[i] = tmp1[i];
	}
	cascadeData.Distance = GetDLight()->GetShadowCascadeDistances();
	
	GetContext()->UpdateSubresource(perSceneCBuffer_.Get(), 0, nullptr, &sceneData_, 0, 0);
	GetContext()->UpdateSubresource(cascadeCBuffer_.Get(), 0, nullptr, &cascadeData, 0, 0);
	
	camera_->UpdateMatrix();
	for (const auto c : components_)
	{
		c->Update();
	}
}

void Game::UpdateInternal()
{
}
