#include "DX11Framework.h"

#include <chrono>

#include "ConfigManager.h"
#include "GameObjectFactory.h"
#include "Helpers.h"
#include "Textures.h"

//#define RETURNFAIL(x) if(FAILED(x)) return x;

//Grabs windows messages
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    auto* framework = reinterpret_cast<DX11Framework*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
        
    case WM_CLOSE:
        if (GetForegroundWindow() == hWnd)
	        DestroyWindow(hWnd);
        
    case WM_SIZE:
    {
        if (!framework) break;

        UINT width = LOWORD(lParam);
        UINT height = HIWORD(lParam);

        framework->PassResizeInfo(wParam, width, height);
        break;
    }

    case WM_EXITSIZEMOVE:
        if (!framework) break;
        framework->ResizeWindow();
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

HRESULT DX11Framework::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    //Enum bool wrapper with error codes.
    HRESULT hr = S_OK;

    hr = CreateWindowHandle(hInstance, nCmdShow);
    if (FAILED(hr)) return E_FAIL;

    hr = CreateD3DDevice();
    if (FAILED(hr)) return E_FAIL;

    hr = CreateSwapChainAndFrameBuffer();
    if (FAILED(hr)) return E_FAIL;

    hr = InitShadersAndInputLayout();
    if (FAILED(hr)) return E_FAIL;

    hr = InitVertexIndexBuffers();
    if (FAILED(hr)) return E_FAIL;

    hr = InitPipelineVariables();
    if (FAILED(hr)) return E_FAIL;

    hr = InitRunTimeData();
    if (FAILED(hr)) return E_FAIL;

    return hr;
}

HRESULT DX11Framework::CreateWindowHandle(HINSTANCE hInstance, int nCmdShow)
{
    const wchar_t* windowName  = L"DX11Framework";

    WNDCLASSW wndClass;
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

    //registers process
    RegisterClassW(&wndClass);

    _windowHandle = CreateWindowExW(0, windowName, windowName, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 
        _screen->Width, _screen->Height, nullptr, nullptr, hInstance, nullptr);
    Helpers::WindowHandle = _windowHandle;
    SetWindowLongPtrA(_windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    return S_OK;
}

HRESULT DX11Framework::CreateD3DDevice()
{
    HRESULT hr = S_OK;

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
    };

    ID3D11Device* baseDevice;
    ID3D11DeviceContext* baseDeviceContext;

    DWORD createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &baseDevice, nullptr, &baseDeviceContext);
    if (FAILED(hr)) return hr;

    ///////////////////////////////////////////////////////////////////////////////////////////////

    hr = baseDevice->QueryInterface(__uuidof(ID3D11Device), reinterpret_cast<void**>(_device.GetAddressOf()));
    hr = baseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext), &_immediateContext);

    baseDevice->Release();
    baseDeviceContext->Release();

    ///////////////////////////////////////////////////////////////////////////////////////////////

    hr = _device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&_dxgiDevice));
    if (FAILED(hr)) return hr;

    IDXGIAdapter* dxgiAdapter;
    hr = _dxgiDevice->GetAdapter(&dxgiAdapter);
    if (FAILED(hr)) return hr;
    hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&_dxgiFactory));
    if (FAILED(hr)) return hr;
    dxgiAdapter->Release();

    return S_OK;
}

HRESULT DX11Framework::CreateSwapChainAndFrameBuffer()
{
    HRESULT hr = S_OK;

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

    hr = _dxgiFactory->CreateSwapChainForHwnd(_device.Get(), _windowHandle, &swapChainDesc, nullptr, nullptr, &_swapChain);
    if (FAILED(hr)) return hr;

    ///////////////////////////////////////////////////////////////////////////////////////////////

    ID3D11Texture2D* frameBuffer = nullptr;

    hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&frameBuffer));
    if (FAILED(hr)) return hr;

    D3D11_RENDER_TARGET_VIEW_DESC frameBufferDesc = {};
    frameBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //sRGB render target enables hardware gamma correction
    frameBufferDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = _device->CreateRenderTargetView(frameBuffer, &frameBufferDesc, &_frameBufferView);

    D3D11_TEXTURE2D_DESC depthBufferDesc = {};
    frameBuffer->GetDesc(&depthBufferDesc);

    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = _device->CreateTexture2D(&depthBufferDesc, nullptr, &_depthStencilBuffer);
    if (FAILED(hr)) return hr;
    hr = _device->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);
    if (FAILED(hr)) return hr;

    frameBuffer->Release();

    //hr = _dxgiFactory->MakeWindowAssociation(_windowHandle, DXGI_MWA_NO_ALT_ENTER);
    //if (FAILED(hr)) return hr;

    return hr;
}

HRESULT DX11Framework::InitShadersAndInputLayout()
{
	HRESULT hr = S_OK;
	#pragma region PhongShader
    {
        
        ID3DBlob* errorBlob;

        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
        // Setting this flag improves the shader debugging experience, but still allows 
        // the shaders to be optimized and to run exactly the way they will run in 
        // the release configuration of this program.
        dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

        ID3DBlob* vsBlob;

        //compiling the shader at runtime
        hr = D3DCompileFromFile(L"Shaders/PhongShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", dwShaderFlags, 0, &vsBlob, &errorBlob);
        if (FAILED(hr))
        {
            MessageBoxA(_windowHandle, static_cast<char*>(errorBlob->GetBufferPointer()), nullptr, ERROR);
            errorBlob->Release();
            return hr;
        }

        hr = _device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &_vertexShaderPhong);

        if (FAILED(hr)) return hr;

        D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
            { "TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }
        };

        hr = _device->CreateInputLayout(
            inputElementDesc,
            ARRAYSIZE(inputElementDesc),
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            &_inputLayout);

        if (FAILED(hr)) return hr;

        ///////////////////////////////////////////////////////////////////////////////////////////////

        ID3DBlob* psBlob;

        hr = D3DCompileFromFile(L"Shaders/PhongShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_5_0", dwShaderFlags, 0, &psBlob, &errorBlob);
        if (FAILED(hr))
        {
            MessageBoxA(_windowHandle, static_cast<char*>(errorBlob->GetBufferPointer()), nullptr, ERROR);
            errorBlob->Release();
            return hr;
        }

        hr = _device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &_pixelShaderPhong);

        vsBlob->Release();
        psBlob->Release();
    }
	#pragma endregion
	#pragma region BlinnPhongShader
	{

        ID3DBlob* errorBlob;

        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
        // Setting this flag improves the shader debugging experience, but still allows 
        // the shaders to be optimized and to run exactly the way they will run in 
        // the release configuration of this program.
        dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

        ID3DBlob* vsBlob;

        //compiling the shader at runtime
        hr = D3DCompileFromFile(L"Shaders/BlinnPhongShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", dwShaderFlags, 0, &vsBlob, &errorBlob);
        if (FAILED(hr))
        {
            MessageBoxA(_windowHandle, static_cast<char*>(errorBlob->GetBufferPointer()), nullptr, ERROR);
            errorBlob->Release();
            return hr;
        }

        hr = _device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &_vertexShaderBlinn);

        if (FAILED(hr)) return hr;

        D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
            { "TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }
        };

        hr = _device->CreateInputLayout(
            inputElementDesc,
            ARRAYSIZE(inputElementDesc),
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            &_inputLayout);

        if (FAILED(hr)) return hr;

        ///////////////////////////////////////////////////////////////////////////////////////////////

        ID3DBlob* psBlob;

        hr = D3DCompileFromFile(L"Shaders/BlinnPhongShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_5_0", dwShaderFlags, 0, &psBlob, &errorBlob);
        if (FAILED(hr))
        {
            MessageBoxA(_windowHandle, static_cast<char*>(errorBlob->GetBufferPointer()), nullptr, ERROR);
            errorBlob->Release();
            return hr;
        }

        hr = _device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &_pixelShaderBlinn);

        vsBlob->Release();
        psBlob->Release();
	}
	#pragma endregion
	#pragma region SkyboxShader
    	{

        ID3DBlob* errorBlob;

        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
        // Setting this flag improves the shader debugging experience, but still allows 
        // the shaders to be optimized and to run exactly the way they will run in 
        // the release configuration of this program.
        dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

        ID3DBlob* vsBlob;

        //compiling the shader at runtime
        hr = D3DCompileFromFile(L"Shaders/SkyboxShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", dwShaderFlags, 0, &vsBlob, &errorBlob);
        if (FAILED(hr))
        {
            MessageBoxA(_windowHandle, static_cast<char*>(errorBlob->GetBufferPointer()), nullptr, ERROR);
            errorBlob->Release();
            return hr;
        }

        hr = _device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &_vertexShaderSkybox);

        if (FAILED(hr)) return hr;

        D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
            { "TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 }
        };

        hr = _device->CreateInputLayout(
            inputElementDesc,
            ARRAYSIZE(inputElementDesc),
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            &_inputLayout);

        if (FAILED(hr)) return hr;

        ///////////////////////////////////////////////////////////////////////////////////////////////

        ID3DBlob* psBlob;

        hr = D3DCompileFromFile(L"Shaders/SkyboxShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_5_0", dwShaderFlags, 0, &psBlob, &errorBlob);
        if (FAILED(hr))
        {
            MessageBoxA(_windowHandle, static_cast<char*>(errorBlob->GetBufferPointer()), nullptr, ERROR);
            errorBlob->Release();
            return hr;
        }

        hr = _device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &_pixelShaderSkybox);

        vsBlob->Release();
        psBlob->Release();
	}
	#pragma endregion
    return hr;
}

HRESULT DX11Framework::InitVertexIndexBuffers()
{
    HRESULT hr = S_OK;

    _skybox = GameObjectFactory::CreateSkybox(_device);

    auto globalLight1 = GameObjectFactory::CreateDirectionalLight(
        {-20, -20, -20},
		{1, 240/255.0f, 240/255.0f},
        1,
        {245.0f/255, 1, 1},
        1,
        {0.05f, 0.05f, 0.05f}
    );

    auto pointLight1 = GameObjectFactory::CreatePointLight(
        {10, 15, 0},
        { 0, 1, 0 },
        2,
        { 0, 1, 0 },
        3,
        { },
        1,
        0, 
        0.1f,
        32,
        _device
    );

    auto pointLight2 = GameObjectFactory::CreatePointLight(
		{50, 10, 10},
        {1, 0, 0},
        3,
        {0.8f, 0, 0},
        4,
        { },
        1,
        0,
        0.01f,
        128,
        _device
    );

    auto pointLight3 = GameObjectFactory::CreatePointLight(
		{-50, 10, 50},
        {1, 111/255.0f, 0},
        5,
        {1, 111/255.0f, 0},
        6,
        { },
        1,
        0,
        0.1f,
        128,
        _device
    );

    auto pointLight4 = GameObjectFactory::CreatePointLight(
		{100, 10, 0},
        {0, 1, 1},
        5,
        {0, 1, 1},
        6,
        { },
        1,
        0.1f,
        0.1f,
        128,
        _device
    );

    _manager.Add(globalLight1);
    _manager.Add(pointLight1);
    _manager.Add(pointLight2);
    _manager.Add(pointLight3);
    _manager.Add(pointLight4);

    auto obj1 = GameObjectFactory::CreateRenderObject(
        "Meshes/suzanne.obj",
        RightHanded,
        "", 
        _device);
    _manager.Add(obj1);
    
    auto obj2 = GameObjectFactory::CreateRenderObject(
        "Meshes/cube2.obj", 
        RightHanded, 
        "Crate", 
        _device);
    obj2->SetParent(obj1);

    auto obj3 = GameObjectFactory::CreateRenderObject(
        "Meshes/cone.obj",
        RightHanded, 
        "Cone", 
        _device);

    auto plane = GameObjectFactory::CreateRenderObject(
		"Meshes/plane.obj",
        RightHanded,
        "Plane",
        _device);

    plane->Transform->SetScale(10, 10, 10);
    plane->Transform->SetPosition(0, -5, 0);

    obj3->SetParent(obj2);

    obj1->Transform->SetScale(5, 5, 5);
    obj2->Transform->SetPosition(15, 0, 0);
    obj2->Transform->SetScale(2, 2, 2);
    obj3->Transform->SetPosition(15, 0, 0);

    _manager.Add(plane);

    return S_OK;
}

HRESULT DX11Framework::InitPipelineVariables()
{
    HRESULT hr = S_OK;

    //Input Assembler
    _immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _immediateContext->IASetInputLayout(_inputLayout);

    //Rasterizer
    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;

    hr = _device->CreateRasterizerState(&rasterizerDesc, &_fillState);
    if (FAILED(hr)) return hr;

    rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    hr = _device->CreateRasterizerState(&rasterizerDesc, &_wireframeState);
    if (FAILED(hr)) return hr;

    _immediateContext->RSSetState(_fillState);

    //Viewport Values
    _viewport = { 0.0f, 0.0f, static_cast<float>(_screen->Width),
    	static_cast<float>(_screen->Height), 0.0f, 1.0f };

    _immediateContext->RSSetViewports(1, &_viewport);

    //Constant Buffer
    D3D11_BUFFER_DESC blinnConstantBufferDesc = {};
    blinnConstantBufferDesc.ByteWidth = sizeof(BlinnConstantBuffer);
    blinnConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    blinnConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    blinnConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = _device->CreateBuffer(&blinnConstantBufferDesc, nullptr, _blinnConstantBuffer.GetAddressOf());
    if (FAILED(hr)) { return hr; }

    _immediateContext->VSSetConstantBuffers(0, 1, _blinnConstantBuffer.GetAddressOf());
    _immediateContext->PSSetConstantBuffers(0, 1, _blinnConstantBuffer.GetAddressOf());

    D3D11_BUFFER_DESC lightBufferDesc = {};
    lightBufferDesc.ByteWidth = sizeof(LightBuffer);
    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = _device->CreateBuffer(&lightBufferDesc, nullptr, _lightBuffer.GetAddressOf());
    if (FAILED(hr)) { return hr; }

    _immediateContext->VSSetConstantBuffers(1, 1, _lightBuffer.GetAddressOf());
    _immediateContext->PSSetConstantBuffers(1, 1, _lightBuffer.GetAddressOf());

    D3D11_BUFFER_DESC phongConstantBufferDesc = {};
    phongConstantBufferDesc.ByteWidth = sizeof(PhongConstantBuffer);
    phongConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    phongConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    phongConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = _device->CreateBuffer(&phongConstantBufferDesc, nullptr, _phongConstantBuffer.GetAddressOf());
    if (FAILED(hr)) { return hr; }

    _immediateContext->VSSetConstantBuffers(2, 1, _phongConstantBuffer.GetAddressOf());
    _immediateContext->PSSetConstantBuffers(2, 1, _phongConstantBuffer.GetAddressOf());


    D3D11_BUFFER_DESC skyboxConstantBufferDesc = {};
    skyboxConstantBufferDesc.ByteWidth = sizeof(SkyboxCBuffer);
    skyboxConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    skyboxConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    skyboxConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = _device->CreateBuffer(&skyboxConstantBufferDesc, nullptr, _skyboxConstantBuffer.GetAddressOf());
    if (FAILED(hr)) { return hr; }

    _immediateContext->VSSetConstantBuffers(3, 1, _skyboxConstantBuffer.GetAddressOf());
    _immediateContext->PSSetConstantBuffers(3, 1, _skyboxConstantBuffer.GetAddressOf());

	D3D11_SAMPLER_DESC bilinearSamplerDesc = {};
    bilinearSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    bilinearSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    bilinearSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    bilinearSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    bilinearSamplerDesc.MaxLOD = 1;
    bilinearSamplerDesc.MinLOD = 0;

    hr = _device->CreateSamplerState(&bilinearSamplerDesc, _bilinearSamplerState.GetAddressOf());
    if (FAILED(hr)) { return hr; }
    _immediateContext->PSSetSamplers(0, 1, _bilinearSamplerState.GetAddressOf());


    D3D11_DEPTH_STENCIL_DESC dsDescSkybox = {};
    dsDescSkybox.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    dsDescSkybox.DepthEnable = true;
    dsDescSkybox.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    hr = _device->CreateDepthStencilState(&dsDescSkybox, &_depthStencilSkybox);
    if (FAILED(hr)) return hr;

	return S_OK;
}

HRESULT DX11Framework::InitRunTimeData()
{
    _manager.Add(GameObjectFactory::CreateLookToCamera(
    	{0, 10, -50},
    	{0, 0, 1},
    	{0, 1, 0},
    	_screen
    ));

    _manager.Add(GameObjectFactory::CreateLookAtCamera(
    {0, 10, -50},
        {0, 0, 0},
        {0, 1, 0},
        _screen
    ));

    _camera = _manager.FindFirstObjectOfType<Camera>(true).value();
    _blinnConstantData.CameraPosition = _camera.lock()->GetEye();

    return S_OK;
}

DX11Framework::~DX11Framework()
{
    if(_dxgiDevice)_dxgiDevice->Release();
    if(_dxgiFactory)_dxgiFactory->Release();
    if(_frameBufferView)_frameBufferView->Release();
    if(_swapChain)_swapChain->Release();

    if(_fillState)_fillState->Release();
    if (_wireframeState)_wireframeState->Release();
    if(_vertexShaderPhong)_vertexShaderPhong->Release();
    if(_inputLayout)_inputLayout->Release();
    if(_pixelShaderPhong)_pixelShaderPhong->Release();

    if(_vertexShaderBlinn)_vertexShaderBlinn->Release();
    if(_pixelShaderBlinn)_pixelShaderBlinn->Release();

    if(_vertexShaderSkybox)_vertexShaderSkybox->Release();
    if(_pixelShaderSkybox)_pixelShaderSkybox->Release();

    if (_depthStencilBuffer)_depthStencilBuffer->Release();
    if (_depthStencilView)_depthStencilView->Release();
}

void DX11Framework::Update()
{
	#pragma region DeltaTime
    static auto clock = std::chrono::high_resolution_clock();
    static auto lastTime = clock.now();
    auto timePoint = clock.now();
    double deltaTime = std::chrono::duration_cast
	    <std::chrono::nanoseconds>(timePoint - lastTime).count()
	    / 1000000000.0;

    lastTime = timePoint;
	#pragma endregion

	#pragma region TitleUpdate
    Helpers::UpdateTitleFPS(deltaTime, _selectedObject, _windowHandle);
	#pragma endregion

    _manager.Update(deltaTime);

    #pragma region Keybinds
    if (GetForegroundWindow() == _windowHandle)
    {
	    if (GetAsyncKeyState(VK_F1) & 0x0001)
	    {
	        ID3D11RasterizerState* currentState;
	        _immediateContext->RSGetState(&currentState);
		    switch (currentState == _wireframeState)
		    {
		    case true:
	            _immediateContext->RSSetState(_fillState);
	            break;
		    case false:
	            _immediateContext->RSSetState(_wireframeState);
	            break;
		    }
	    }

        if (GetAsyncKeyState(VK_F2) & 0x0001 && !_selectedObject.expired())
        _manager.Remove(_selectedObject.lock());

	    if (GetAsyncKeyState(VK_F3) & 0x0001)
		    _useBlinnPhong = !_useBlinnPhong;

	    if (GetAsyncKeyState('2') & 0x0001)
	    {
		    auto cameras = _manager.FindObjectsOfType<Camera>(true);
    		if (cameras.size() == 1 && _camera.lock() != cameras.front())
        		_camera = cameras.front();
	        else
	        {
		        for (int i = 0; i < cameras.size(); i++)
		        {
			        if (cameras[i] == _camera.lock())
				        if (i != cameras.size() - 1)
					        _camera = cameras[i + 1];
		        }
		    }
	    }

	    if (GetAsyncKeyState('1') & 0x0001)
	    {
		    auto cameras = _manager.FindObjectsOfType<Camera>(true);
	        if (cameras.size() == 1 && _camera.lock() != cameras.front())
        		_camera = cameras.front();
	        else
	        {
		        for (int i = 0; i < cameras.size(); i++)
		        {
			        if (cameras[i] == _camera.lock() && i != 0)
				        _camera = cameras[i - 1];
		        }
	        }
	    }

	    if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
	        WndProc(_windowHandle, WM_CLOSE, 0, 0);

        if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
        {
        	if (GetAsyncKeyState(VK_RIGHT) & 0x0001)
	        {
		        if (_selectedObject.expired() && _manager.RootObjects.size() > 0)
	                _selectedObject = _manager.RootObjects.front();
	            else if (!_selectedObject.lock()->GetParent())
	            {
	                bool next = false;
	                for (auto obj : _manager.RootObjects)
	                {
                        if (next)
	                    {
		                    _selectedObject = obj;
                            break;
	                    }

		                if (obj == _selectedObject.lock())
	                        next = true;
	                }
	            }
	            else if (_selectedObject.lock()->GetParent())
	            {
		            auto parent = _selectedObject.lock()->GetParent();
	                for (int i = 0; i < parent->GetChildren().size(); i++)
	                {
		                if (parent->GetChildren()[i] == _selectedObject.lock() && i < parent->GetChildren().size() - 1)
		                {
			                _selectedObject = parent->GetChildren()[i + 1];
	                        break;
		                }
	                }
	            }
	        }
	        if (GetAsyncKeyState(VK_LEFT) & 0x0001)
	        {
		        if (!_selectedObject.expired())
		        {
			        if (!_selectedObject.lock()->GetParent())
			        {
				        if (_selectedObject.lock() != _manager.RootObjects.front())
				        {
					        std::shared_ptr<GameObject> lastObj;
		                    for (auto obj : _manager.RootObjects)
		                    {
	                            if (obj == _selectedObject.lock())
	                            {
                            		_selectedObject = lastObj;
	                                break;
	                            }
			                    lastObj = obj;
		                    }
				        }
			        }
	                else if (_selectedObject.lock()->GetParent())
	                {
		                auto parent = _selectedObject.lock()->GetParent();
		                for (int i = 0; i < parent->GetChildren().size(); i++)
		                {
			                if (parent->GetChildren()[i] == _selectedObject.lock() && i != 0)
			                {
				                _selectedObject = parent->GetChildren()[i - 1];
		                        break;
			                }
		                }
	                }
		        }
	        }
	        if (GetAsyncKeyState(VK_UP) & 0x0001)
	        {
		        if (!_selectedObject.expired() && _selectedObject.lock()->GetParent())
			        _selectedObject = _selectedObject.lock()->GetParent();
	        }
	        if (GetAsyncKeyState(VK_DOWN) & 0x0001)
	        {
		        if (!_selectedObject.expired() && _selectedObject.lock()->GetChildren().size() > 0)
	                _selectedObject = _selectedObject.lock()->GetChildren().front();
	        }

            if (GetAsyncKeyState('0') & 0x0001)
            {
	            _selectedObject = {};
            }
        }

		if (GetAsyncKeyState(VK_LSHIFT) & 0x8000 && !_selectedObject.expired())
		{
            auto objConf = ConfigManager::GetConfig()->ObjectManipulationConfig;
			if (GetAsyncKeyState('W') & 0x8000)
				_selectedObject.lock()->Transform->AddToPosition(0, 0, deltaTime * objConf.MovementMultiplier);
			if (GetAsyncKeyState('S') & 0x8000)
                _selectedObject.lock()->Transform->AddToPosition(0, 0, -deltaTime * objConf.MovementMultiplier);
            if (GetAsyncKeyState('A') & 0x8000)
                _selectedObject.lock()->Transform->AddToPosition(-deltaTime * objConf.MovementMultiplier, 0, 0);
            if (GetAsyncKeyState('D') & 0x8000)
                _selectedObject.lock()->Transform->AddToPosition(deltaTime * objConf.MovementMultiplier, 0, 0);
            if (GetAsyncKeyState(VK_SPACE) & 0x8000)
                _selectedObject.lock()->Transform->AddToPosition(0, deltaTime * objConf.MovementMultiplier, 0);
            if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
                _selectedObject.lock()->Transform->AddToPosition(0, -deltaTime * objConf.MovementMultiplier, 0);

            if (GetAsyncKeyState('Q') & 0x8000)
				_selectedObject.lock()->Transform->AddToRotation(0, 0, deltaTime * objConf.RotationMultiplier);
			if (GetAsyncKeyState('E') & 0x8000)    
                _selectedObject.lock()->Transform->AddToRotation(0, 0, -deltaTime * objConf.RotationMultiplier);
            if (GetAsyncKeyState('R') & 0x8000)    
                _selectedObject.lock()->Transform->AddToRotation(deltaTime * objConf.RotationMultiplier, 0, 0);
            if (GetAsyncKeyState('F') & 0x8000)    
                _selectedObject.lock()->Transform->AddToRotation(-deltaTime * objConf.RotationMultiplier, 0, 0);
            if (GetAsyncKeyState('X') & 0x8000)    
				_selectedObject.lock()->Transform->AddToRotation(0, deltaTime * objConf.RotationMultiplier, 0);
			if (GetAsyncKeyState('Z') & 0x8000)    
                _selectedObject.lock()->Transform->AddToRotation(0, -deltaTime * objConf.RotationMultiplier, 0);
		}

    }
	#pragma endregion

	Mesh::ClearUnusedMeshes();
    Material::ClearUnusedMaterials();
    Textures::ClearUnusedTextures();
}

void DX11Framework::Draw()
{
    //Present unbinds render target, so rebind and clear at start of each frame
    float backgroundColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };
    _immediateContext->OMSetRenderTargets(1, &_frameBufferView, _depthStencilView);
    _immediateContext->ClearRenderTargetView(_frameBufferView, backgroundColor);
    _immediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);

	if (_camera.expired())
	{
		if (auto cameras = _manager.FindFirstObjectOfType<Camera>(true); cameras.has_value())
			_camera = cameras.value();
        else
        {
	        float noCameraColor[4] = { 1.0f, 0.0f, 1.0f, 1.0f};
	        _immediateContext->ClearRenderTargetView(_frameBufferView, noCameraColor);
			_swapChain->Present(0, 0);
	        return;
        }
	}

    _lightBufferData.ActiveLightCount = 0;
    for (auto lights = _manager.FindObjectsOfType<Light>(); std::shared_ptr<Light> light : lights)
    {
	    _lightBufferData.PointLights[_lightBufferData.ActiveLightCount] = light->GetLightData();
        _lightBufferData.ActiveLightCount++;
    }

    //Set object variables and draw
    if (_useBlinnPhong)
    {
        _blinnConstantData.CameraPosition = _camera.lock()->GetEye();
	    _blinnConstantData.View = XMMatrixTranspose(XMLoadFloat4x4(&_camera.lock()->GetView()));
	    _blinnConstantData.Projection = XMMatrixTranspose(XMLoadFloat4x4(&_camera.lock()->GetProjection()));

	    _immediateContext->VSSetShader(_vertexShaderBlinn, nullptr, 0);
	    _immediateContext->PSSetShader(_pixelShaderBlinn, nullptr, 0);
        _manager.Draw(_immediateContext, _blinnConstantBuffer, _lightBuffer, _blinnConstantData, _lightBufferData);
    }
    else
    {
	    for (auto light : _lightBufferData.PointLights)
	    {
			if (light.LinearAttenuation == 0 && light.QuadraticAttenuation == 0 && light.LightRadius == 0)
			{
                _phongConstantData.GlobalLight = light;
                break;
			}
	    }

        _phongConstantData.CameraPosition = _camera.lock()->GetEye();
        _phongConstantData.View = XMMatrixTranspose(XMLoadFloat4x4(&_camera.lock()->GetView()));
        _phongConstantData.Projection = XMMatrixTranspose(XMLoadFloat4x4(&_camera.lock()->GetProjection()));

	    _immediateContext->VSSetShader(_vertexShaderPhong, nullptr, 0);
	    _immediateContext->PSSetShader(_pixelShaderPhong, nullptr, 0);
        _manager.Draw(_immediateContext, _phongConstantBuffer, _phongConstantData);
    }
    
    _immediateContext->VSSetShader(_vertexShaderSkybox, nullptr, 0);
    _immediateContext->PSSetShader(_pixelShaderSkybox, nullptr, 0);
    _immediateContext->OMSetDepthStencilState(_depthStencilSkybox, 0);
    _skybox->Draw(_immediateContext, _skyboxConstantBuffer, _skyboxConstantData, _camera.lock());
    _immediateContext->OMSetDepthStencilState(nullptr, 0);

    //Present Back buffer to screen
    _swapChain->Present(0, 0);
}

HRESULT DX11Framework::PassResizeInfo(WPARAM resizeType, UINT width, UINT height)
{
    //https://learn.microsoft.com/en-gb/windows/win32/direct3ddxgi/d3d10-graphics-programming-guide-dxgi

    HRESULT hr = S_OK;
    _screen->Width = static_cast<int>(width);
    _screen->Height = static_cast<int>(height);

    BOOL fsState;
    hr = _swapChain->GetFullscreenState(&fsState, nullptr);
    if (FAILED(hr)) return hr;

    if (resizeType == SIZE_MAXIMIZED || fsState != _screen->Fullscreen)
    {
	    _screen->Fullscreen = fsState;
        return ResizeWindow();
    }

    return hr;
}

HRESULT DX11Framework::ResizeWindow()
{
    //https://learn.microsoft.com/en-gb/windows/win32/direct3ddxgi/d3d10-graphics-programming-guide-dxgi

	_immediateContext->OMSetRenderTargets(0, nullptr, nullptr);
	_frameBufferView->Release();
	_depthStencilView->Release();
	_depthStencilBuffer->Release();

	DXGI_MODE_DESC dxgiModeDesc = {};
    dxgiModeDesc.Width = _screen->Width;
    dxgiModeDesc.Height = _screen->Height;
    HRESULT hr = _swapChain->ResizeTarget(&dxgiModeDesc);
    if (FAILED(hr)) { return hr; }

	hr = _swapChain->ResizeBuffers(0, _screen->Width, _screen->Height, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hr)) { return hr; }

	ID3D11Texture2D* buffer;
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&buffer));
	if (FAILED(hr)) { return hr; }

	D3D11_RENDER_TARGET_VIEW_DESC bufferDesc = {};
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    bufferDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	hr = _device->CreateRenderTargetView(buffer, &bufferDesc, &_frameBufferView);
	if (FAILED(hr)) { return hr; }

	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	buffer->GetDesc(&depthBufferDesc);
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = _device->CreateTexture2D(&depthBufferDesc, nullptr, &_depthStencilBuffer);
	if (FAILED(hr)) return hr;
	hr = _device->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);
	if (FAILED(hr)) { return hr; }

	buffer->Release();

	_immediateContext->OMSetRenderTargets(1, &_frameBufferView, _depthStencilView);

	_viewport.Width = static_cast<float>(_screen->Width);
	_viewport.Height = static_cast<float>(_screen->Height);
    _viewport.MinDepth = 0.0f;
    _viewport.MaxDepth = 1.0f;
    _viewport.TopLeftX = 0;
    _viewport.TopLeftY = 0;

	_immediateContext->RSSetViewports(1, &_viewport);

	float aspect = _viewport.Width / _viewport.Height;

    for (auto camera : _manager.FindObjectsOfType<Camera>())
    {
	    XMMATRIX perspective = XMMatrixPerspectiveFovLH(XMConvertToRadians(ConfigManager::GetConfig()->CameraConfig.FieldOfView), 
            aspect, _camera.lock()->GetNearDepth(),camera->GetFarDepth());
		XMStoreFloat4x4(&camera->GetProjection(), perspective);
    }

    return S_OK;
}