#pragma once
#include <chrono>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <map>
#include <memory>
#include <string>
#include <windows.h>
#include <wrl.h>

#include "SceneGraph.h"
#include "Screen.h"
#include "Shaders.h"

using Microsoft::WRL::ComPtr;

class SimpleEngine
{
public:
	SimpleEngine() = default;

	HRESULT Initialize(HINSTANCE hInstance, int nShowCmd);

	void Update();
	void FixedUpdate(double fixedDeltaTime) {}
	void Draw();

	void OnWindowSizeChanged(WPARAM resizeType, UINT width, UINT height);
	void OnWindowSizeChangeComplete();

private:
	HRESULT CreateWindowHandle(HINSTANCE hInstance);
	HRESULT CreateD3DDevice();
	HRESULT CreateSwapChain();
	HRESULT CreateFrameBuffer();
	HRESULT InitializeShaders();
	HRESULT InitializePipeline();
	HRESULT InitializeRunTimeData();

	HRESULT InitializeVertexShaderLayout(ID3DBlob* vsBlob);
	ComPtr<ID3D11VertexShader> CompileVertexShader(LPCWSTR path);
	ComPtr<ID3D11PixelShader> CompilePixelShader(LPCWSTR path);
	HRESULT InitializeRasterizerStates();
	HRESULT InitializeBuffers();
	HRESULT InitializeSamplers();
	HRESULT InitializeDepthStencils();

	HWND _hwnd;

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<IDXGIDevice> _dxgiDevice;
	ComPtr<IDXGIFactory2> _dxgiFactory;
	ComPtr<IDXGISwapChain1> _swapChain;
	ComPtr<ID3D11RenderTargetView> _frameBufferView;
	ComPtr<ID3D11Texture2D> _depthStencilBuffer;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;
	ComPtr<ID3D11InputLayout> _inputLayout;
	D3D11_VIEWPORT _viewport;

	std::chrono::time_point<std::chrono::steady_clock> _lastFrameTime = std::chrono::high_resolution_clock::now();
	double _timeSinceLastFixedUpdate = 0;
	std::map<std::string, Shaders> _shaders = {};
	std::unique_ptr<Screen> _screen = std::make_unique<Screen>(800, 600);
	std::unique_ptr<SceneGraph> _sceneGraph = nullptr;
};

