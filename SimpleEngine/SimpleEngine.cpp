#include "SimpleEngine.h"
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <filesystem>

#include "Configuration.h"
#include "DataStore.h"

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

HRESULT SimpleEngine::Initialize(HINSTANCE hInstance, int nShowCmd)
{
	HRESULT hr = S_OK;

	hr = CreateWindowHandle(hInstance, nShowCmd);
	if (FAILED(hr)) return E_FAIL;

	hr = CreateD3DDevice();
	if (FAILED(hr)) return E_FAIL;

	hr = CreateSwapChain();
	if (FAILED(hr)) return E_FAIL;

	hr = CreateFrameBuffer();
	if (FAILED(hr)) return E_FAIL;

	hr = InitializeShaders();
	if (FAILED(hr)) return E_FAIL;

	hr = InitializePipeline();
	if (FAILED(hr)) return E_FAIL;

	hr = InitializeRunTimeData();
	if (FAILED(hr)) return E_FAIL;

	return hr;
}

HRESULT SimpleEngine::CreateWindowHandle(HINSTANCE hInstance, int nShowCmd)
{
	const wchar_t* windowName = L"SimpleEngine";

	WNDCLASSW wndClass;
	wndClass.style = 0;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = nullptr;
	wndClass.hIcon = nullptr;
	wndClass.hCursor = nullptr;
	wndClass.hbrBackground = nullptr;
	wndClass.lpszClassName = windowName;

	RegisterClassW(&wndClass);

	_hwnd = CreateWindowExW(0, windowName, windowName, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 
        _screen->Width, _screen->Height, nullptr, nullptr, hInstance, nullptr);

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
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
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

	//hr = _dxgiFactory->MakeWindowAssociation(_windowHandle, DXGI_MWA_NO_ALT_ENTER);
    //if (FAILED(hr)) return hr;

	return hr;
}

HRESULT SimpleEngine::InitializeShaders()
{
	std::string path = "/Assets/Shaders";

	if (!std::filesystem::is_directory(path))
		return E_FAIL;

	for (const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(path))
	{
		if (directoryEntry.is_directory() || directoryEntry.path().extension() != ".hlsl")
			continue;

		//TODO: Shader Compilation
		const std::filesystem::path& fsPath = directoryEntry.path();

		if (std::string filePath = fsPath.filename().string(); filePath.find("VS_") == 1)
		{
			ComPtr<ID3D11VertexShader> vs = CompileVertexShader(directoryEntry.path().c_str());
			if (!vs)
				return E_FAIL;

			ShaderData vsData = { vs, Vertex };
			DataStore::VertexShaders.Store(fsPath.string(), vsData);
		}
		else if (filePath.find("PS_") == 1)
		{
			ComPtr<ID3D11PixelShader> ps = CompilePixelShader(directoryEntry.path().c_str());
			if (!ps)
				return E_FAIL;

			ShaderData psData = { ps, Pixel };
			DataStore::PixelShaders.Store(fsPath.string(), psData);
		}
	}

	return S_OK;
}

HRESULT SimpleEngine::InitializePipeline()
{
	HRESULT hr = S_OK;

	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_context->IASetInputLayout(_inputLayout.Get());

	hr = InitializeRasterizerStates();
	if (FAILED(hr)) return hr;

	_viewport = { 0.0f, 0.0f, static_cast<float>(_screen->Width), static_cast<float>(_screen->Height), 0.0f, 1.0f};
	_context->RSSetViewports(1, &_viewport);
}

HRESULT SimpleEngine::InitializeVertexShaderLayout(ID3DBlob* vsBlob)
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
		errorBlob->Release();
		vsBlob->Release();
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
		vsBlob->Release();
		errorBlob->Release();
		return nullptr;
	}

	if (_inputLayout == nullptr)
	{
		hr = InitializeVertexShaderLayout(vsBlob);
		if (FAILED(hr)) return {};
	}

	vsBlob->Release();
	errorBlob->Release();

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
		errorBlob->Release();
		psBlob->Release();
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
		psBlob->Release();
		errorBlob->Release();
		return nullptr;
	}

	psBlob->Release();
	errorBlob->Release();

	return ps;
}

HRESULT SimpleEngine::InitializeRasterizerStates()
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
