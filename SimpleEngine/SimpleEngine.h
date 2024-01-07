#pragma once
#include <d3d11.h>
#include <dxgi1_2.h>
#include <memory>
#include <windows.h>
#include <wrl.h>

#include "Screen.h"

using Microsoft::WRL::ComPtr;

class SimpleEngine
{
public:
	SimpleEngine() = default;

	HRESULT Initialize(HINSTANCE hInstance, int nShowCmd);

	void Update();
	void FixedUpdate();
	void Draw();

	void OnWindowSizeChanged(WPARAM wParam, UINT width, UINT height);
	void OnWindowSizeChangeComplete();

private:
	HRESULT CreateWindowHandle(HINSTANCE hInstance, int nShowCmd);
	HRESULT CreateD3DDevice();
	HRESULT CreateSwapChain();
	HRESULT CreateFrameBuffer();
	HRESULT InitializeShaders();
	HRESULT InitializePipeline();
	HRESULT InitializeRunTimeData();

	HWND _hwnd;

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<IDXGIDevice> _dxgiDevice;
	ComPtr<IDXGIFactory2> _dxgiFactory;
	ComPtr<IDXGISwapChain1> _swapChain;
	ComPtr<ID3D11RenderTargetView> _frameBufferView;
	ComPtr<ID3D11Texture2D> _depthStencilBuffer;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;


	std::unique_ptr<Screen> _screen;
};

