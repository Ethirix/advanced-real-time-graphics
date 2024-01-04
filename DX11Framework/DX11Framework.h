#pragma once

#include <windows.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "RenderObject.h"
#include "Camera.h"
#include "GameObjectManager.h"
#include "PhongConstantBuffer.h"
#include "Skybox.h"
#include "SkyboxCBuffer.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class DX11Framework
{
	ComPtr<ID3D11DeviceContext> _immediateContext = nullptr;
	ComPtr<ID3D11Device> _device;
	IDXGIDevice* _dxgiDevice = nullptr;
	IDXGIFactory2* _dxgiFactory = nullptr;
	ID3D11RenderTargetView* _frameBufferView = nullptr;
	IDXGISwapChain1* _swapChain = nullptr;
	D3D11_VIEWPORT _viewport = {};

	ID3D11RasterizerState* _fillState = nullptr;
	ID3D11RasterizerState* _wireframeState = nullptr;
	ID3D11InputLayout* _inputLayout = nullptr;

	ID3D11VertexShader* _vertexShaderPhong = nullptr;
	ID3D11PixelShader* _pixelShaderPhong = nullptr;

	ID3D11VertexShader* _vertexShaderBlinn = nullptr;
	ID3D11PixelShader* _pixelShaderBlinn = nullptr;

	ID3D11VertexShader* _vertexShaderSkybox = nullptr;
	ID3D11PixelShader* _pixelShaderSkybox = nullptr;

	ComPtr<ID3D11Buffer> _phongConstantBuffer;
	ComPtr<ID3D11Buffer> _blinnConstantBuffer;
	ComPtr<ID3D11Buffer> _skyboxConstantBuffer;
	ComPtr<ID3D11Buffer> _lightBuffer;

	ComPtr<ID3D11SamplerState> _bilinearSamplerState;

	HWND _windowHandle = nullptr;

	GameObjectManager _manager;
	std::shared_ptr<Skybox> _skybox;

	std::weak_ptr<GameObject> _selectedObject;

	std::weak_ptr<Camera> _camera = {};
	std::shared_ptr<Screen> _screen = std::make_shared<Screen>(1280, 768);

	PhongConstantBuffer _phongConstantData = {};
	BlinnConstantBuffer _blinnConstantData = {};
	SkyboxCBuffer _skyboxConstantData = {};
	LightBuffer _lightBufferData = {};

	bool _useBlinnPhong = true;

	ID3D11Texture2D* _depthStencilBuffer = nullptr;
	ID3D11DepthStencilView* _depthStencilView = nullptr;
	ID3D11DepthStencilState* _depthStencilSkybox = nullptr;

public:
	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);
	HRESULT CreateWindowHandle(HINSTANCE hInstance, int nCmdShow);
	HRESULT CreateD3DDevice();
	HRESULT CreateSwapChainAndFrameBuffer();
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexIndexBuffers();
	HRESULT InitPipelineVariables();
	HRESULT InitRunTimeData();
	~DX11Framework();
	void Update();
	void Draw();

	HRESULT PassResizeInfo(WPARAM resizeType, UINT width, UINT height);
	HRESULT ResizeWindow();
};