#include "SimpleEngine.h"
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "Buffers.h"
#include "CBObjectCameraData.h"
#include "Configuration.h"
#include "Constants.h"
#include "DataStore.h"
#include "LightComponent.h"
#include "SceneGraph.h"
#include "Screen.h"

LRESULT CALLBACK WndProc(const HWND hwnd, const UINT message, const WPARAM wParam, const LPARAM lParam)  
{
	PAINTSTRUCT ps;
	HDC hdc;

	auto* engine = reinterpret_cast<SimpleEngine*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		if (GetForegroundWindow() == hwnd)
			DestroyWindow(hwnd);
		break;
	case WM_SIZE:
	{
		if (!engine) break;

		UINT width = LOWORD(lParam);
		UINT height = HIWORD(lParam);

		engine->OnWindowSizeChanged(wParam, width, height);
		break;
	}
	case WM_EXITSIZEMOVE:
		if (!engine) break;

		engine->OnWindowSizeChangeComplete();
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

HRESULT SimpleEngine::Initialise(HINSTANCE hInstance, int nShowCmd)
{
	HRESULT hr = S_OK;

	hr = CreateWindowHandle(hInstance);
	if (FAILED(hr)) return E_FAIL;

	hr = CreateD3DDevice();
	if (FAILED(hr)) return E_FAIL;

	hr = CreateSwapChain();
	if (FAILED(hr)) return E_FAIL;

	hr = CreateFrameBuffer();
	if (FAILED(hr)) return E_FAIL;

	hr = InitialiseShaders();
	if (FAILED(hr)) return E_FAIL;

	hr = InitialisePipeline();
	if (FAILED(hr)) return E_FAIL;

	hr = InitialiseRunTimeData();
	if (FAILED(hr)) return E_FAIL;

	return hr;
}

HRESULT SimpleEngine::CreateWindowHandle(HINSTANCE hInstance)
{
	const wchar_t* windowName = L"SimpleEngine";

	WNDCLASSW wndClass {};
	wndClass.style = 0;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = nullptr;
	wndClass.hIcon = nullptr;
	wndClass.hCursor = nullptr;
	wndClass.hbrBackground = nullptr;
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = windowName;

	RegisterClassW(&wndClass);

	_hwnd = CreateWindowExW(0, windowName, windowName, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 
		Screen::Width, Screen::Height, nullptr, nullptr, hInstance, nullptr);

    SetWindowLongPtrA(_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	return S_OK;
}

HRESULT SimpleEngine::CreateD3DDevice()
{
	HRESULT hr = S_OK;

	D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* baseDevice;
	ID3D11DeviceContext* baseDeviceContext;

	DWORD createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT | createDeviceFlags, 
		featureLevels,
		ARRAYSIZE(featureLevels), 
		D3D11_SDK_VERSION, 
		&baseDevice, 
		nullptr, 
		&baseDeviceContext
	);

    if (FAILED(hr)) return hr;

	hr = baseDevice->QueryInterface(__uuidof(ID3D11Device), reinterpret_cast<void**>(_device.GetAddressOf()));
	if (FAILED(hr)) return hr;
    hr = baseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext), &_context);
	if (FAILED(hr)) return hr;

	baseDevice->Release();
    baseDeviceContext->Release();

	hr = _device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(_dxgiDevice.GetAddressOf()));
    if (FAILED(hr)) return hr;

    IDXGIAdapter* dxgiAdapter;
    hr = _dxgiDevice->GetAdapter(&dxgiAdapter);
    if (FAILED(hr)) return hr;
    hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(_dxgiFactory.GetAddressOf()));
    if (FAILED(hr)) return hr;
    dxgiAdapter->Release();

	return hr;
}

HRESULT SimpleEngine::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc {};
	swapChainDesc.Width = 0; // Defer to WindowWidth
    swapChainDesc.Height = 0; // Defer to WindowHeight
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //FLIP* modes don't support sRGB back buffer
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags = 0;

	return _dxgiFactory->CreateSwapChainForHwnd(
		_device.Get(),
		_hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		_swapChain.GetAddressOf()
	);
}

HRESULT SimpleEngine::CreateFrameBuffer()
{
	HRESULT hr = S_OK;

	ID3D11Texture2D* frameBuffer = nullptr;

	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&frameBuffer));
    if (FAILED(hr)) return hr;

	D3D11_RENDER_TARGET_VIEW_DESC frameBufferDesc = {};
	frameBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	frameBufferDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	hr = _device->CreateRenderTargetView(frameBuffer, &frameBufferDesc, _frameBufferView.GetAddressOf());
	if (FAILED(hr)) return hr;

	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	frameBuffer->GetDesc(&depthBufferDesc);
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = _device->CreateTexture2D(&depthBufferDesc, nullptr, _depthStencilBuffer.GetAddressOf());
    if (FAILED(hr)) return hr;
    hr = _device->CreateDepthStencilView(_depthStencilBuffer.Get(), nullptr, _depthStencilView.GetAddressOf());
    if (FAILED(hr)) return hr;

    frameBuffer->Release();

	hr = _dxgiFactory->MakeWindowAssociation(_hwnd, DXGI_MWA_NO_ALT_ENTER);
    if (FAILED(hr)) return hr;

	return hr;
}

HRESULT SimpleEngine::InitialiseShaders()
{
	std::string path = "Assets/Shaders";

	if (!std::filesystem::is_directory(path))
		return E_FAIL;

	for (const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(path))
	{
		if (directoryEntry.is_directory() || directoryEntry.path().extension() != ".hlsl")
			continue;

		const std::filesystem::path& fsPath = directoryEntry.path();

		if (std::string filePath = fsPath.filename().string(); filePath.find("VS_") != std::string::npos)
		{
			ComPtr<ID3D11VertexShader> vs = CompileVertexShader(directoryEntry.path().c_str());
			if (!vs)
				return E_FAIL;

			ShaderData vsData = { vs, Vertex };
			std::string vsPath = fsPath.string();
			std::ranges::replace(vsPath, '\\', '/');
			DataStore::VertexShaders.Store(vsPath, vsData);
		}
		else if (filePath.find("PS_") != std::string::npos)
		{
			ComPtr<ID3D11PixelShader> ps = CompilePixelShader(directoryEntry.path().c_str());
			if (!ps)
				return E_FAIL;

			ShaderData psData = { ps, Pixel };
			std::string psPath = fsPath.string();
			std::ranges::replace(psPath, '\\', '/');
			DataStore::PixelShaders.Store(psPath, psData);
		}
	}

	return S_OK;
}

HRESULT SimpleEngine::InitialisePipeline()
{
	HRESULT hr = S_OK;

	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_context->IASetInputLayout(_inputLayout.Get());

	hr = InitialiseRasterizerStates();
	if (FAILED(hr)) return hr;

	_viewport = { 0.0f, 0.0f,
		static_cast<float>(Screen::Width),
		static_cast<float>(Screen::Height),
		0.0f, 1.0f};
	_context->RSSetViewports(1, &_viewport);

	hr = InitialiseBuffers();
	if (FAILED(hr)) return hr;

	hr = InitialiseSamplers();
	if (FAILED(hr)) return hr;

	hr = InitialiseDepthStencils();
	if (FAILED(hr)) return hr;

	return hr;
}

HRESULT SimpleEngine::InitialiseRunTimeData()
{
	SceneGraph::Initialize(
		"Assets/Configuration/SceneGraph.json", _device);

	if (auto cameraComponent = SceneGraph::TryGetComponentFromObjects<CameraComponent>();
		cameraComponent.has_value())
		_camera = cameraComponent.value();

	OnWindowSizeChangeComplete();

	return S_OK;
}

HRESULT SimpleEngine::InitialiseVertexShaderLayout(ID3DBlob* vsBlob)
{
	D3D11_INPUT_ELEMENT_DESC vsInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }
	};

	return _device->CreateInputLayout(
		vsInputLayout,
		ARRAYSIZE(vsInputLayout),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		_inputLayout.GetAddressOf()
	);
}

ComPtr<ID3D11VertexShader> SimpleEngine::CompileVertexShader(LPCWSTR path)
{
	ComPtr<ID3D11VertexShader> vs = {};
	ID3DBlob* errorBlob;
	ID3DBlob* vsBlob;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

	HRESULT hr = D3DCompileFromFile(
		path, 
		nullptr, 
		D3D_COMPILE_STANDARD_FILE_INCLUDE, 
		"VS_Main", 
		"vs_5_0", 
		shaderFlags, 
		0, 
		&vsBlob, 
		&errorBlob
	);

	if (FAILED(hr))
	{
		MessageBoxA(_hwnd, 
			static_cast<char*>(errorBlob->GetBufferPointer()), 
			nullptr, 
			ERROR
		);
		if (vsBlob) vsBlob->Release();
		if (errorBlob) errorBlob->Release();
		return nullptr;
	}

	hr = _device->CreateVertexShader(
		vsBlob->GetBufferPointer(), 
		vsBlob->GetBufferSize(), 
		nullptr, 
		vs.GetAddressOf()
	);
	if (FAILED(hr)) 
	{
		if (vsBlob) vsBlob->Release();
		if (errorBlob) errorBlob->Release();
		return nullptr;
	}

	if (_inputLayout == nullptr)
	{
		hr = InitialiseVertexShaderLayout(vsBlob);
		if (FAILED(hr)) return {};
	}

	if (vsBlob) vsBlob->Release();
	if (errorBlob) errorBlob->Release();

	return vs;
}

ComPtr<ID3D11PixelShader> SimpleEngine::CompilePixelShader(LPCWSTR path)
{
	ComPtr<ID3D11PixelShader> ps;
	ID3DBlob* psBlob;
	ID3DBlob* errorBlob;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

	HRESULT hr = D3DCompileFromFile(
		path,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS_Main",
		"ps_5_0",
		shaderFlags,
		0,
		&psBlob,
		&errorBlob
	);

	if (FAILED(hr))
	{
		MessageBoxA(_hwnd,
			static_cast<char*>(errorBlob->GetBufferPointer()),
			nullptr,
			ERROR
		);
		if (psBlob) psBlob->Release();
		if (errorBlob) errorBlob->Release();
		return nullptr;
	}

	hr = _device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		nullptr,
		ps.GetAddressOf()
	);

	if (FAILED(hr))
	{
		if (psBlob) psBlob->Release();
		if (errorBlob) errorBlob->Release();
		return nullptr;
	}

	if (psBlob) psBlob->Release();
	if (errorBlob) errorBlob->Release();

	return ps;
}

HRESULT SimpleEngine::InitialiseRasterizerStates()
{
	HRESULT hr = S_OK;

	for (auto& [key, description] : 
		Configuration::RasterizerStateConfig.RasterizerDescriptions)
	{
		ComPtr<ID3D11RasterizerState> state;
		hr = _device->CreateRasterizerState(&description, state.GetAddressOf());
		if (FAILED(hr)) return hr;

		DataStore::RasterizerStates.Store(key, state);
	}

	if (auto state = DataStore::RasterizerStates.Retrieve(
		Configuration::RasterizerStateConfig.DefaultRasterizerStateKey); state.has_value())
		_context->RSSetState(state.value().Get());
	else
		return E_FAIL;

	return hr;
}

HRESULT SimpleEngine::InitialiseBuffers()
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDescription = {};
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

#pragma region CBObjectCameraData
	bufferDescription.ByteWidth = sizeof(CBObjectCameraData);
	hr = _device->CreateBuffer(&bufferDescription, nullptr, Buffers::CBObjectCameraData.Buffer.GetAddressOf());
	if (FAILED(hr)) return hr;

	_context->VSSetConstantBuffers(0, 1, Buffers::CBObjectCameraData.Buffer.GetAddressOf());
	_context->PSSetConstantBuffers(0, 1, Buffers::CBObjectCameraData.Buffer.GetAddressOf());
#pragma endregion

#pragma region CBMaterial
	bufferDescription.ByteWidth = sizeof(CBMaterial);
	hr = _device->CreateBuffer(&bufferDescription, nullptr, Buffers::CBMaterial.Buffer.GetAddressOf());
	if (FAILED(hr)) return hr;

	_context->VSSetConstantBuffers(1, 1, Buffers::CBMaterial.Buffer.GetAddressOf());
	_context->PSSetConstantBuffers(1, 1, Buffers::CBMaterial.Buffer.GetAddressOf());
#pragma endregion

#pragma region CBTextures
	bufferDescription.ByteWidth = sizeof(CBTextures);
	hr = _device->CreateBuffer(&bufferDescription, nullptr, Buffers::CBTextures.Buffer.GetAddressOf());
	if (FAILED(hr)) return hr;

	_context->VSSetConstantBuffers(2, 1, Buffers::CBTextures.Buffer.GetAddressOf());
	_context->PSSetConstantBuffers(2, 1, Buffers::CBTextures.Buffer.GetAddressOf());
#pragma endregion

#pragma region CBLighting
	bufferDescription.ByteWidth = sizeof(CBLighting);
	hr = _device->CreateBuffer(&bufferDescription, nullptr, Buffers::CBLighting.Buffer.GetAddressOf());
	if (FAILED(hr)) return hr;

	_context->VSSetConstantBuffers(3, 1, Buffers::CBLighting.Buffer.GetAddressOf());
	_context->PSSetConstantBuffers(3, 1, Buffers::CBLighting.Buffer.GetAddressOf());
#pragma endregion

	return hr;
}

HRESULT SimpleEngine::InitialiseSamplers()
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC bilinearSamplerDesc = {};
	bilinearSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    bilinearSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    bilinearSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    bilinearSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    bilinearSamplerDesc.MaxLOD = 1;
    bilinearSamplerDesc.MinLOD = 0;

	ComPtr<ID3D11SamplerState> bilinearSampler = {};
	hr = _device->CreateSamplerState(&bilinearSamplerDesc, bilinearSampler.GetAddressOf());
    if (FAILED(hr)) { return hr; }
    _context->PSSetSamplers(0, 1, bilinearSampler.GetAddressOf());
	DataStore::SamplerStates.Store(BILINEAR_SAMPLER_KEY, bilinearSampler);

	return hr;
}

HRESULT SimpleEngine::InitialiseDepthStencils()
{
	HRESULT hr = S_OK;

	D3D11_DEPTH_STENCIL_DESC dsSkyboxDesc = {};
	dsSkyboxDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsSkyboxDesc.DepthEnable = true;
	dsSkyboxDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	ComPtr<ID3D11DepthStencilState> skyboxDepthStencil = {};
	hr = _device->CreateDepthStencilState(&dsSkyboxDesc, skyboxDepthStencil.GetAddressOf());
	if (FAILED(hr)) { return hr; }
	DataStore::DepthStencilStates.Store(SKYBOX_DEPTH_STENCIL_KEY, skyboxDepthStencil);

	return hr;
}

void SimpleEngine::OnWindowSizeChanged(WPARAM resizeType, UINT width, UINT height)
{
	Screen::Width = static_cast<int>(width);
	Screen::Height = static_cast<int>(height);

	BOOL fsState;
	HRESULT hr = _swapChain->GetFullscreenState(&fsState, nullptr);

	if (resizeType == SIZE_MAXIMIZED || fsState != Screen::Fullscreen)
	{
		Screen::Fullscreen = fsState;
		return OnWindowSizeChangeComplete();
	}
}

void SimpleEngine::OnWindowSizeChangeComplete()
{
	_context->OMSetRenderTargets(0, nullptr, nullptr);
	_frameBufferView->Release();
	_depthStencilView->Release();
	_depthStencilBuffer->Release();

	DXGI_MODE_DESC dxgiModeDesc {};
	dxgiModeDesc.Width = Screen::Width;
	dxgiModeDesc.Height = Screen::Height;
	HRESULT hr = _swapChain->ResizeTarget(&dxgiModeDesc);
	hr = _swapChain->ResizeBuffers(0, Screen::Width, Screen::Height, DXGI_FORMAT_UNKNOWN, 0);
	ID3D11Texture2D* buffer {};
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&buffer));

	D3D11_RENDER_TARGET_VIEW_DESC bufferDesc = {};
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    bufferDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	hr = _device->CreateRenderTargetView(buffer, &bufferDesc, _frameBufferView.GetAddressOf());

	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	buffer->GetDesc(&depthBufferDesc);
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = _device->CreateTexture2D(&depthBufferDesc, nullptr, _depthStencilBuffer.GetAddressOf());
	hr = _device->CreateDepthStencilView(_depthStencilBuffer.Get(), nullptr, _depthStencilView.GetAddressOf());

	buffer->Release();

	_context->OMSetRenderTargets(1, _frameBufferView.GetAddressOf(), _depthStencilView.Get());

	_viewport.Width = static_cast<float>(Screen::Width);
	_viewport.Height = static_cast<float>(Screen::Height);
    _viewport.MinDepth = 0.0f;
    _viewport.MaxDepth = 1.0f;
    _viewport.TopLeftX = 0;
    _viewport.TopLeftY = 0;

	_context->RSSetViewports(1, &_viewport);

	float aspect = _viewport.Width / _viewport.Height;

    for (std::weak_ptr<CameraComponent> camera : SceneGraph::GetComponentsFromObjects<CameraComponent>())
    {
	    DirectX::XMMATRIX perspective = DirectX::XMMatrixPerspectiveFovLH(
		    DirectX::XMConvertToRadians(camera.lock()->GetFieldOfView()), aspect, 
			_camera.lock()->GetNearDepth(), camera.lock()->GetFarDepth());
		XMStoreFloat4x4(&camera.lock()->GetProjection(), perspective);
    }
}

void SimpleEngine::Update()
{
	#pragma region DeltaTime
	auto timePoint = std::chrono::high_resolution_clock::now();
	double deltaTime = std::chrono::duration_cast
		<std::chrono::nanoseconds>(timePoint - _lastFrameTime).count()
		/ 1000000000.0;

	_lastFrameTime = timePoint;

	_timeSinceLastFixedUpdate += deltaTime;
	if (_timeSinceLastFixedUpdate >= PHYSICS_TIMESTEP)
	{
		FixedUpdate(PHYSICS_TIMESTEP);
		_timeSinceLastFixedUpdate -= PHYSICS_TIMESTEP;
	}
	#pragma endregion

	if (_camera.expired())
	{
		if (auto cameraComponent = SceneGraph::TryGetComponentFromObjects<CameraComponent>(); cameraComponent.has_value())
			_camera = cameraComponent.value();
	}

	auto lightComponents = SceneGraph::GetComponentsFromObjects<LightComponent>();
	Buffers::CBLighting.BufferData.ActiveLightCount = lightComponents.size();
	for (int i = 0; i < lightComponents.size() && i < MAX_LIGHTS; i++)
	{
		Buffers::CBLighting.BufferData.PointLights[i] = lightComponents[i].lock()->Light;
	}

	//BIG HACK NEED TO GET A CONCRETE WAY TO GET THE SKYBOX!
	SceneGraph::GetObjectAtPosition(4)->Transform->SetPosition(_camera.lock()->GameObject.lock()->Transform->GetPosition());
	
	SceneGraph::Update(deltaTime);
}

void SimpleEngine::FixedUpdate(double fixedDeltaTime)
{
	SceneGraph::FixedUpdate(fixedDeltaTime);
}

void SimpleEngine::Draw()
{
	if (_camera.expired())
	{
		float errorColour[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
		_context->OMSetRenderTargets(1, _frameBufferView.GetAddressOf(), _depthStencilView.Get());
		_context->ClearRenderTargetView(_frameBufferView.Get(), errorColour);
		_context->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);

		_swapChain->Present(0,0);
		return;
	}

	float backgroundColour[4] = { 0.025f, 0.025f, 0.025f, 1.0f };
	_context->OMSetRenderTargets(1, _frameBufferView.GetAddressOf(), _depthStencilView.Get());
	_context->ClearRenderTargetView(_frameBufferView.Get(), backgroundColour);
	_context->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);

	SceneGraph::Draw(_context);

	_swapChain->Present(0, 0);
}