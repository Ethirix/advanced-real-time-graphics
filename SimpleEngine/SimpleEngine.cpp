#include "SimpleEngine.h"

#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <filesystem>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <nlohmann/json.hpp>

#include "Buffers.h"
#include "CBObjectCameraData.h"
#include "Configuration.h"
#include "Constants.h"
#include "DataStore.h"
#include "GlobalDefines.h"
#include "Helpers.h"
#include "imgui.h"
#include "LightManager.h"
#include "MeshComponent.h"
#include "SceneGraph.h"
#include "Screen.h"

LRESULT CALLBACK WndProc(const HWND hwnd, const UINT message, const WPARAM wParam, const LPARAM lParam)  
{
	PAINTSTRUCT ps;
	HDC hdc;

	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
		return true;

	SimpleEngine* engine = reinterpret_cast<SimpleEngine*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
		
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
	HRESULT hr = CreateWindowHandle(hInstance); FAIL_CHECK
	hr = CreateD3DDevice(); FAIL_CHECK
	hr = CreateSwapChain(); FAIL_CHECK
	hr = CreateFrameBuffers(); FAIL_CHECK
	hr = InitialiseShaders(); FAIL_CHECK
	hr = InitialisePipeline(); FAIL_CHECK
	hr = InitialiseRunTimeData(); FAIL_CHECK
	hr = InitialiseImGUI(); FAIL_CHECK 

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
	D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_1
	};

	ID3D11Device* baseDevice;
	ID3D11DeviceContext* baseDeviceContext;

	DWORD createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDevice(
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
	); FAIL_CHECK

	hr = baseDevice->QueryInterface(IID_PPV_ARGS(&_device)); FAIL_CHECK
    hr = baseDeviceContext->QueryInterface(IID_PPV_ARGS(&_context)); FAIL_CHECK

	baseDevice->Release();
    baseDeviceContext->Release();

	hr = _device->QueryInterface(IID_PPV_ARGS(&_dxgiDevice)); FAIL_CHECK

    IDXGIAdapter* dxgiAdapter;
    hr = _dxgiDevice->GetAdapter(&dxgiAdapter); FAIL_CHECK
    hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&_dxgiFactory)); FAIL_CHECK
    dxgiAdapter->Release();

	hr = _dxgiFactory->MakeWindowAssociation(_hwnd, DXGI_MWA_NO_ALT_ENTER); FAIL_CHECK

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

HRESULT SimpleEngine::CreateFrameBuffers()
{
	ID3D11Texture2D* frameBuffer = nullptr;
	HRESULT hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(&frameBuffer)); FAIL_CHECK

	D3D11_RENDER_TARGET_VIEW_DESC frameBufferDesc = {};
	frameBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	frameBufferDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = _device->CreateRenderTargetView(frameBuffer, &frameBufferDesc, _frameBufferView.GetAddressOf()); FAIL_CHECK

	D3D11_TEXTURE2D_DESC depthBufferDesc{};
	depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
#ifdef _DEFERRED_RENDER
	depthBufferDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
#endif
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.Width = Screen::Width;
	depthBufferDesc.Height = Screen::Height;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc{};
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = _device->CreateTexture2D(&depthBufferDesc, nullptr, _depthStencilBuffer.GetAddressOf()); FAIL_CHECK
    hr = _device->CreateDepthStencilView(_depthStencilBuffer.Get(), &depthStencilDesc, _depthStencilView.GetAddressOf()); FAIL_CHECK

    frameBuffer->Release();

	D3D11_TEXTURE2D_DESC textureDesc{};
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};

	textureDesc.Width = Screen::Width;
	textureDesc.Height = Screen::Height;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	
	textureDesc.SampleDesc.Count = 1;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;

	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

#ifdef _DEFERRED_RENDER

	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	renderTargetViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;

	hr = _device->CreateTexture2D(&textureDesc, nullptr, _depthLinearTexture.GetAddressOf()); FAIL_CHECK
	hr = _device->CreateRenderTargetView(_depthLinearTexture.Get(), &renderTargetViewDesc, _depthLinearFrameBufferView.GetAddressOf()); FAIL_CHECK
	hr = _device->CreateShaderResourceView(_depthLinearTexture.Get(), &shaderResourceViewDesc, _depthLinearShaderResourceView.GetAddressOf()); FAIL_CHECK

#endif

	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	hr = _device->CreateTexture2D(&textureDesc, nullptr, _baseOutputTexture.GetAddressOf()); FAIL_CHECK
	hr = _device->CreateRenderTargetView(_baseOutputTexture.Get(), &renderTargetViewDesc, _baseOutputBufferView.GetAddressOf()); FAIL_CHECK
	hr = _device->CreateShaderResourceView(_baseOutputTexture.Get(), &shaderResourceViewDesc, _baseOutputShaderResourceView.GetAddressOf()); FAIL_CHECK

	hr = _device->CreateTexture2D(&textureDesc, nullptr, _outputCopyTexture.GetAddressOf()); FAIL_CHECK
	hr = _device->CreateShaderResourceView(_outputCopyTexture.Get(), &shaderResourceViewDesc, _outputCopyShaderResourceView.GetAddressOf()); FAIL_CHECK

	for (auto& mesh : SceneGraph::GetComponentsFromObjects<MeshComponent>())
	{
		if (Texture& diffuse = mesh.lock()->Textures->Diffuse; diffuse.Name == "NO_TEXTURE")
		{
			diffuse.Resource = _outputCopyShaderResourceView;
			diffuse.Slot = 0;
		}
	}

#ifdef _DEFERRED_RENDER

	hr = _device->CreateTexture2D(&textureDesc, nullptr, _albedoTexture.GetAddressOf()); FAIL_CHECK
	hr = _device->CreateRenderTargetView(_albedoTexture.Get(), &renderTargetViewDesc, _albedoFrameBufferView.GetAddressOf()); FAIL_CHECK
	hr = _device->CreateShaderResourceView(_albedoTexture.Get(), &shaderResourceViewDesc, _albedoShaderResourceView.GetAddressOf()); FAIL_CHECK

	textureDesc.Format = DXGI_FORMAT_R11G11B10_FLOAT;
	renderTargetViewDesc.Format = DXGI_FORMAT_R11G11B10_FLOAT;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R11G11B10_FLOAT;

	hr = _device->CreateTexture2D(&textureDesc, nullptr, &_lightingDiffuseTexture); FAIL_CHECK
	hr = _device->CreateRenderTargetView(_lightingDiffuseTexture.Get(), &renderTargetViewDesc, _lightingDiffuseFrameBufferView.GetAddressOf()); FAIL_CHECK
	hr = _device->CreateShaderResourceView(_lightingDiffuseTexture.Get(), &shaderResourceViewDesc, _lightingDiffuseShaderResourceView.GetAddressOf()); FAIL_CHECK

	hr = _device->CreateTexture2D(&textureDesc, nullptr, &_lightingSpecularTexture); FAIL_CHECK
	hr = _device->CreateRenderTargetView(_lightingSpecularTexture.Get(), &renderTargetViewDesc, _lightingSpecularFrameBufferView.GetAddressOf()); FAIL_CHECK
	hr = _device->CreateShaderResourceView(_lightingSpecularTexture.Get(), &shaderResourceViewDesc, _lightingSpecularShaderResourceView.GetAddressOf()); FAIL_CHECK

	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	renderTargetViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

	hr = _device->CreateTexture2D(&textureDesc, nullptr, &_normalTexture); FAIL_CHECK
	hr = _device->CreateRenderTargetView(_normalTexture.Get(), &renderTargetViewDesc, _normalFrameBufferView.GetAddressOf()); FAIL_CHECK
	hr = _device->CreateShaderResourceView(_normalTexture.Get(), &shaderResourceViewDesc, _normalShaderResourceView.GetAddressOf()); FAIL_CHECK

	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTargetViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	hr = _device->CreateTexture2D(&textureDesc, nullptr, &_worldPositionTexture); FAIL_CHECK
	hr = _device->CreateRenderTargetView(_worldPositionTexture.Get(), &renderTargetViewDesc, _worldPositionFrameBufferView.GetAddressOf()); FAIL_CHECK
	hr = _device->CreateShaderResourceView(_worldPositionTexture.Get(), &shaderResourceViewDesc, _worldPositionShaderResourceView.GetAddressOf()); FAIL_CHECK

#endif

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
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_context->IASetInputLayout(_inputLayout.Get());

	HRESULT hr = InitialiseRasterizerStates(); FAIL_CHECK

	_viewport = { 0.0f, 0.0f,
		static_cast<float>(Screen::Width),
		static_cast<float>(Screen::Height),
		0.0f, 1.0f};
	_context->RSSetViewports(1, &_viewport);

	hr = InitialiseBuffers(); FAIL_CHECK
	hr = InitialiseSamplers(); FAIL_CHECK
	hr = InitialiseDepthStencils(); FAIL_CHECK

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

	_screenQuad = std::make_unique<ScreenQuad>(_device);

	return S_OK;
}

HRESULT SimpleEngine::InitialiseImGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplWin32_Init(_hwnd);
	ImGui_ImplDX11_Init(_device.Get(), _context.Get());

	return S_OK;
}

HRESULT SimpleEngine::InitialiseVertexShaderLayout(const ComPtr<ID3DBlob>& vsBlob)
{
	D3D11_INPUT_ELEMENT_DESC vsInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }
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
	ComPtr<ID3DBlob> errorBlob;
	ComPtr<ID3DBlob> vsBlob;

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
		vsBlob.GetAddressOf(), 
		errorBlob.GetAddressOf()
	);

	if (FAILED(hr))
	{
		MessageBoxA(_hwnd, static_cast<char*>(errorBlob->GetBufferPointer()), nullptr, ERROR);
		return nullptr;
	}

	hr = _device->CreateVertexShader(
		vsBlob->GetBufferPointer(), 
		vsBlob->GetBufferSize(), 
		nullptr, 
		vs.GetAddressOf()
	);
	if (FAILED(hr)) return nullptr;

	if (_inputLayout == nullptr)
	{
		hr = InitialiseVertexShaderLayout(vsBlob);
		if (FAILED(hr)) return nullptr;
	}

	return vs;
}

ComPtr<ID3D11PixelShader> SimpleEngine::CompilePixelShader(LPCWSTR path)
{
	ComPtr<ID3D11PixelShader> ps;
	ComPtr<ID3DBlob> errorBlob;
	ComPtr<ID3DBlob> psBlob;

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
		psBlob.GetAddressOf(),
		errorBlob.GetAddressOf()
	);

	if (FAILED(hr))
	{
		MessageBoxA(_hwnd, static_cast<char*>(errorBlob->GetBufferPointer()), nullptr, ERROR);
		return nullptr;
	}

	hr = _device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		nullptr,
		ps.GetAddressOf()
	);
	if (FAILED(hr)) return nullptr;

	return ps;
}

HRESULT SimpleEngine::InitialiseRasterizerStates()
{
	HRESULT hr = S_OK;

	for (auto& [key, description] : 
		Configuration::RasterizerStateConfig.RasterizerDescriptions)
	{
		ComPtr<ID3D11RasterizerState> state;
		hr = _device->CreateRasterizerState(&description, state.GetAddressOf()); FAIL_CHECK

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
	D3D11_BUFFER_DESC bufferDescription = {};
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

#pragma region CBObjectCameraData
	bufferDescription.ByteWidth = sizeof(CBObjectCameraData);
	HRESULT hr = _device->CreateBuffer(&bufferDescription, nullptr, Buffers::CBObjectCameraData.Buffer.GetAddressOf()); FAIL_CHECK

	_context->VSSetConstantBuffers(0, 1, Buffers::CBObjectCameraData.Buffer.GetAddressOf());
	_context->PSSetConstantBuffers(0, 1, Buffers::CBObjectCameraData.Buffer.GetAddressOf());
#pragma endregion

#pragma region CBMaterial
	bufferDescription.ByteWidth = sizeof(CBMaterial);
	hr = _device->CreateBuffer(&bufferDescription, nullptr, Buffers::CBMaterial.Buffer.GetAddressOf()); FAIL_CHECK

	_context->VSSetConstantBuffers(1, 1, Buffers::CBMaterial.Buffer.GetAddressOf());
	_context->PSSetConstantBuffers(1, 1, Buffers::CBMaterial.Buffer.GetAddressOf());
#pragma endregion

#pragma region CBTextures
	bufferDescription.ByteWidth = sizeof(CBTextures);
	hr = _device->CreateBuffer(&bufferDescription, nullptr, Buffers::CBTextures.Buffer.GetAddressOf()); FAIL_CHECK

	_context->VSSetConstantBuffers(2, 1, Buffers::CBTextures.Buffer.GetAddressOf());
	_context->PSSetConstantBuffers(2, 1, Buffers::CBTextures.Buffer.GetAddressOf());
#pragma endregion

#pragma region CBLighting
	bufferDescription.ByteWidth = sizeof(CBLighting);
	hr = _device->CreateBuffer(&bufferDescription, nullptr, Buffers::CBLighting.Buffer.GetAddressOf()); FAIL_CHECK

	_context->VSSetConstantBuffers(3, 1, Buffers::CBLighting.Buffer.GetAddressOf());
	_context->PSSetConstantBuffers(3, 1, Buffers::CBLighting.Buffer.GetAddressOf());
#pragma endregion

#pragma region SRVDirectionalLights
	bufferDescription.ByteWidth = sizeof(DirectionalLightData) * MAX_DIRECTIONAL_LIGHTS;
	bufferDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDescription.StructureByteStride = sizeof(DirectionalLightData);

	hr = _device->CreateBuffer(&bufferDescription, nullptr, Buffers::SRVDirectionalLights.Buffer.GetAddressOf()); FAIL_CHECK

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDirectionalLightDesc{};
	srvDirectionalLightDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDirectionalLightDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDirectionalLightDesc.Buffer.NumElements = MAX_DIRECTIONAL_LIGHTS;

	hr = _device->CreateShaderResourceView(Buffers::SRVDirectionalLights.Buffer.Get(), &srvDirectionalLightDesc,
	                                       Buffers::SRVDirectionalLights.Resource.GetAddressOf()); FAIL_CHECK

	_context->VSSetShaderResources(8, 1, Buffers::SRVDirectionalLights.Resource.GetAddressOf());
	_context->PSSetShaderResources(8, 1, Buffers::SRVDirectionalLights.Resource.GetAddressOf());
#pragma endregion

#pragma region SRVPointLights
	bufferDescription.ByteWidth = sizeof(PointLightData) * MAX_POINT_LIGHTS;
	bufferDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDescription.StructureByteStride = sizeof(PointLightData);

	hr = _device->CreateBuffer(&bufferDescription, nullptr, Buffers::SRVPointLights.Buffer.GetAddressOf()); FAIL_CHECK

	D3D11_SHADER_RESOURCE_VIEW_DESC srvPointLightDesc{};
	srvPointLightDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvPointLightDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvPointLightDesc.Buffer.NumElements = MAX_POINT_LIGHTS;

	hr = _device->CreateShaderResourceView(Buffers::SRVPointLights.Buffer.Get(), &srvPointLightDesc,
	                                       Buffers::SRVPointLights.Resource.GetAddressOf()); FAIL_CHECK

	_context->VSSetShaderResources(9, 1, Buffers::SRVPointLights.Resource.GetAddressOf());
	_context->PSSetShaderResources(9, 1, Buffers::SRVPointLights.Resource.GetAddressOf());
#pragma endregion

#pragma region SRVSpotLights
	bufferDescription.ByteWidth = sizeof(SpotLightData) * MAX_SPOT_LIGHTS;
	bufferDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDescription.StructureByteStride = sizeof(SpotLightData);

	hr = _device->CreateBuffer(&bufferDescription, nullptr, Buffers::SRVSpotLights.Buffer.GetAddressOf()); FAIL_CHECK

	D3D11_SHADER_RESOURCE_VIEW_DESC srvSpotLightDesc{};
	srvSpotLightDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvSpotLightDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvSpotLightDesc.Buffer.NumElements = MAX_SPOT_LIGHTS;

	hr = _device->CreateShaderResourceView(Buffers::SRVSpotLights.Buffer.Get(), &srvSpotLightDesc,
	                                       Buffers::SRVSpotLights.Resource.GetAddressOf()); FAIL_CHECK

	_context->VSSetShaderResources(10, 1, Buffers::SRVSpotLights.Resource.GetAddressOf());
	_context->PSSetShaderResources(10, 1, Buffers::SRVSpotLights.Resource.GetAddressOf());
#pragma endregion

	return hr;
}

HRESULT SimpleEngine::InitialiseSamplers()
{
	D3D11_SAMPLER_DESC bilinearSamplerDesc = {};
	bilinearSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    bilinearSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    bilinearSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    bilinearSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    bilinearSamplerDesc.MaxLOD = 1;
    bilinearSamplerDesc.MinLOD = 0;

	ComPtr<ID3D11SamplerState> bilinearSampler = {};
	HRESULT hr = _device->CreateSamplerState(&bilinearSamplerDesc, bilinearSampler.GetAddressOf()); FAIL_CHECK
    _context->PSSetSamplers(0, 1, bilinearSampler.GetAddressOf());
	DataStore::SamplerStates.Store(BILINEAR_SAMPLER_KEY, bilinearSampler);

	return hr;
}

HRESULT SimpleEngine::InitialiseDepthStencils()
{
	D3D11_DEPTH_STENCIL_DESC dsSkyboxDesc = {};
	dsSkyboxDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsSkyboxDesc.DepthEnable = true;
	dsSkyboxDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	ComPtr<ID3D11DepthStencilState> skyboxDepthStencil = {};
	HRESULT hr = _device->CreateDepthStencilState(&dsSkyboxDesc, skyboxDepthStencil.GetAddressOf()); FAIL_CHECK
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

	//Reinitialise by recreating the RTVs
	hr = CreateFrameBuffers();

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
	while (_timeSinceLastFixedUpdate >= PHYSICS_TIMESTEP)
	{
		FixedUpdate(PHYSICS_TIMESTEP);
		_timeSinceLastFixedUpdate -= PHYSICS_TIMESTEP;
	}
	#pragma endregion

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (_camera.expired())
	{
		if (auto cameraComponent = SceneGraph::TryGetComponentFromObjects<CameraComponent>(); cameraComponent.has_value())
			_camera = cameraComponent.value();
	}

	LightManager::UpdateLights(_context);

	SceneGraph::Update(deltaTime);
}

void SimpleEngine::FixedUpdate(double fixedDeltaTime)
{
	#pragma region Force Keybind Logic
	/*
	 *	SQUARE BRACKETS - Switch Object
	 *		[-] - Previous or Next Object in current scope
	 *	KEYPAD - Force and Force Position
	 *		KP_7 - X-Force Up
	 *		KP_1 - X-Force Down
	 *		KP_8 - Y-Force Up
	 *		KP_2 - Y-Force Down
	 *		KP_9 - Z-Force Up
	 *		KP_3 - Z-Force Down
	 *			LEFT ALT Modifier - Change the Force Position in same way
	 *	GENERAL KEYS
	 *		TAB - Unselect Object
	 *		KP_DECIMAL - Zero Force
	 *		KP_0 - Zero Force Position
	 *		F - Apply Force
	*/

	if (GetAsyncKeyState(VK_TAB) & 0x0001)
		_selectedObject = -1;

	if (GetAsyncKeyState(VK_OEM_6) & 0x0001 && _selectedObject < SceneGraph::Size() - 1)
		_selectedObject += 1;
	if (GetAsyncKeyState(VK_OEM_4) & 0x0001 && _selectedObject > -1)
		_selectedObject -= 1;

	if (GetAsyncKeyState(VK_DECIMAL) & 0x0001)
		_forcePosition = Vector3::Zero();
	if (GetAsyncKeyState(VK_NUMPAD0) & 0x0001)
		_force = Vector3::Zero();
		

	if (GetAsyncKeyState(VK_MENU))
	{
		if (GetAsyncKeyState(VK_NUMPAD7) & 0x8000)
			_forcePosition.X += fixedDeltaTime;
		if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000)
			_forcePosition.X -= fixedDeltaTime;
		if (GetAsyncKeyState(VK_NUMPAD8) & 0x8000)
			_forcePosition.Y += fixedDeltaTime;
		if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000)
			_forcePosition.Y -= fixedDeltaTime;
		if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000)
			_forcePosition.Z += fixedDeltaTime;
		if (GetAsyncKeyState(VK_NUMPAD3) & 0x8000)
			_forcePosition.Z -= fixedDeltaTime;
	}
	else
	{
		if (GetAsyncKeyState(VK_NUMPAD7) & 0x8000)
			_force.X += fixedDeltaTime;
		if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000)
			_force.X -= fixedDeltaTime;
		if (GetAsyncKeyState(VK_NUMPAD8) & 0x8000)
			_force.Y += fixedDeltaTime;
		if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000)
			_force.Y -= fixedDeltaTime;
		if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000)
			_force.Z += fixedDeltaTime;
		if (GetAsyncKeyState(VK_NUMPAD3) & 0x8000)
			_force.Z -= fixedDeltaTime;
	}

	std::weak_ptr<GameObject> obj = SceneGraph::GetObjectAtPosition(_selectedObject);
	if (GetAsyncKeyState('F') & 0x8000)
	{
		if (!obj.expired())
		{
			if (auto phys = obj.lock()->GetComponent<PhysicsComponent>(); !phys.expired())
			{
				phys.lock()->AddRelativeForce(_force, _forcePosition);
			}
		}
	}
	#pragma endregion

	Helpers::UpdateTitleBar(fixedDeltaTime, obj, _force, _forcePosition, _hwnd);

	SceneGraph::FixedUpdate(fixedDeltaTime);
}


void SimpleEngine::Draw()
{
	constexpr float backgroundColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	constexpr float errorColour[4]      = { 1.0f, 0.0f, 1.0f, 1.0f };
	constexpr ID3D11ShaderResourceView* unbind = nullptr;

	_context->CopyResource(_outputCopyTexture.Get(), _baseOutputTexture.Get());

	_context->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
	_context->ClearRenderTargetView(_frameBufferView.Get(), !_camera.expired() ? backgroundColour : errorColour);
	_context->ClearRenderTargetView(_baseOutputBufferView.Get(), backgroundColour);

#ifdef _DEFERRED_RENDER
	_context->ClearRenderTargetView(_albedoFrameBufferView.Get(), backgroundColour);
	_context->ClearRenderTargetView(_normalFrameBufferView.Get(), backgroundColour);
	_context->ClearRenderTargetView(_depthLinearFrameBufferView.Get(), backgroundColour);
	_context->ClearRenderTargetView(_worldPositionFrameBufferView.Get(), backgroundColour);
	_context->ClearRenderTargetView(_lightingDiffuseFrameBufferView.Get(), backgroundColour);
	_context->ClearRenderTargetView(_lightingSpecularFrameBufferView.Get(), backgroundColour);

#pragma region G-Buffer Pass
	ID3D11RenderTargetView* rTVsGPass[4] = {
		_albedoFrameBufferView.Get(), _normalFrameBufferView.Get(), _depthLinearFrameBufferView.Get(),
		_worldPositionFrameBufferView.Get()
	};
	_context->OMSetRenderTargets(4, rTVsGPass, _depthStencilView.Get());

	_context->VSSetShader(DataStore::VertexShaders.Retrieve("Assets/Shaders/VS_GeometryPass.hlsl").value().Shader.Get(),
	                      nullptr, 0);
	_context->PSSetShader(DataStore::PixelShaders.Retrieve("Assets/Shaders/PS_GeometryPass.hlsl").value().Shader.Get(),
						  nullptr, 0);

	SceneGraph::Draw(_context);

	_context->OMSetRenderTargets(0, nullptr, nullptr);
#pragma endregion

#pragma region Lighting Pass
	ID3D11RenderTargetView* rTVsLPass[2] = { _lightingDiffuseFrameBufferView.Get(), _lightingSpecularFrameBufferView.Get() };
	ID3D11ShaderResourceView* sRVsLPass[4] = {
		_albedoShaderResourceView.Get(), _normalShaderResourceView.Get(), _depthLinearShaderResourceView.Get(),
		_worldPositionShaderResourceView.Get()
	};
	_context->PSSetShaderResources(16, 4, sRVsLPass);
	_context->OMSetRenderTargets(2, rTVsLPass, nullptr);

	_context->VSSetShader(DataStore::VertexShaders.Retrieve("Assets/Shaders/VS_ScreenQuad.hlsl").value().Shader.Get(),
		nullptr, 0);
	_context->PSSetShader(DataStore::PixelShaders.Retrieve("Assets/Shaders/PS_LightingPass.hlsl").value().Shader.Get(),
		nullptr, 0);

	_screenQuad->Draw(_context);

	_context->PSSetShaderResources(16, 1, &unbind);
	_context->PSSetShaderResources(17, 1, &unbind);
	_context->PSSetShaderResources(18, 1, &unbind);
	_context->PSSetShaderResources(19, 1, &unbind);
	_context->OMSetRenderTargets(0, nullptr, nullptr);
#pragma endregion

#pragma region Combine Passes
	_context->OMSetRenderTargets(1, _baseOutputBufferView.GetAddressOf(), nullptr);
	ID3D11ShaderResourceView* sRVsGLPass[2] = { _lightingDiffuseShaderResourceView.Get(), _lightingSpecularShaderResourceView.Get() };
	ID3D11ShaderResourceView* sRVsGLPassT[2] = { _albedoShaderResourceView.Get(), _normalShaderResourceView.Get() };
	_context->PSSetShaderResources(16, 2, sRVsGLPassT);
	_context->PSSetShaderResources(20, 2, sRVsGLPass);

	_context->VSSetShader(DataStore::VertexShaders.Retrieve("Assets/Shaders/VS_ScreenQuad.hlsl").value().Shader.Get(),
		nullptr, 0);
	_context->PSSetShader(DataStore::PixelShaders.Retrieve("Assets/Shaders/PS_CombineGeometryLighting.hlsl").value().Shader.Get(),
		nullptr, 0);

	_screenQuad->Draw(_context);

	_context->PSSetShaderResources(16, 1, &unbind);
	_context->PSSetShaderResources(17, 1, &unbind);
	_context->PSSetShaderResources(20, 1, &unbind);
	_context->PSSetShaderResources(21, 1, &unbind);

	_context->OMSetRenderTargets(0, nullptr, nullptr);
#pragma endregion

#else
	_context->OMSetRenderTargets(1, _baseOutputBufferView.GetAddressOf(), _depthStencilView.Get());

	SceneGraph::Draw(_context);

	_context->OMSetRenderTargets(0, nullptr, nullptr);
#endif

#pragma region Display Framebuffer
	_context->OMSetRenderTargets(1, _frameBufferView.GetAddressOf(), nullptr);

	_context->PSSetShaderResources(24, 1, _baseOutputShaderResourceView.GetAddressOf());
	_context->VSSetShader(DataStore::VertexShaders.Retrieve("Assets/Shaders/VS_ScreenQuad.hlsl").value().Shader.Get(),
		nullptr, 0);
	_context->PSSetShader(DataStore::PixelShaders.Retrieve("Assets/Shaders/PS_RenderSRV.hlsl").value().Shader.Get(),
		nullptr, 0);

	_screenQuad->Draw(_context);

	_context->PSSetShaderResources(24, 1, &unbind);
	_context->OMSetRenderTargets(0, nullptr, nullptr);
#pragma endregion

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	_swapChain->Present(0, 0);
}


//void SimpleEngine::Draw()
//{
//	constexpr float backgroundColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
//	constexpr float errorColour[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
//
//	_context->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
//	_context->ClearRenderTargetView(_frameBufferView.Get(), !_camera.expired() ? backgroundColour : errorColour);
//
//
//
//	ImGui::Render();
//	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
//
//	_swapChain->Present(0, 0);
//}
