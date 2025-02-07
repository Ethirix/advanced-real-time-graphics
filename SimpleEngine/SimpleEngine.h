#pragma once
#include <chrono>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <memory>
#include <windows.h>
#include <wrl.h>

#include "CameraComponent.h"
#include "SceneGraph.h"
#include "ScreenQuad.h"

using Microsoft::WRL::ComPtr;

class SimpleEngine
{
public:
	SimpleEngine() = default;

	HRESULT Initialise(HINSTANCE hInstance, int nShowCmd);

	void Update();
	void FixedUpdate(double fixedDeltaTime);
	void Draw();

	void OnWindowSizeChanged(WPARAM resizeType, UINT width, UINT height);
	void OnWindowSizeChangeComplete();

private:
	HRESULT CreateWindowHandle(HINSTANCE hInstance);
	HRESULT CreateD3DDevice();
	HRESULT CreateSwapChain();
	HRESULT CreateFrameBuffers();
	HRESULT InitialiseShaders();
	HRESULT InitialisePipeline();
	HRESULT InitialiseRunTimeData();
	HRESULT InitialiseImGUI();

	HRESULT InitialiseVertexShaderLayout(const ComPtr<ID3DBlob>& vsBlob);
	ComPtr<ID3D11VertexShader> CompileVertexShader(LPCWSTR path);
	ComPtr<ID3D11PixelShader> CompilePixelShader(LPCWSTR path);
	HRESULT InitialiseRasterizerStates();
	HRESULT InitialiseBuffers();
	HRESULT InitialiseSamplers();
	HRESULT InitialiseDepthStencils();

	HWND _hwnd;

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<IDXGIDevice> _dxgiDevice;
	ComPtr<IDXGIFactory2> _dxgiFactory;
	ComPtr<IDXGISwapChain1> _swapChain;

#ifdef _DEFERRED_RENDER
	ComPtr<ID3D11Texture2D> _albedoTexture;
	ComPtr<ID3D11Texture2D> _normalTexture;
	ComPtr<ID3D11Texture2D> _worldPositionTexture;
	ComPtr<ID3D11Texture2D> _lightingDiffuseTexture;
	ComPtr<ID3D11Texture2D> _lightingSpecularTexture;
	ComPtr<ID3D11ShaderResourceView> _albedoShaderResourceView;
	ComPtr<ID3D11ShaderResourceView> _normalShaderResourceView;
	ComPtr<ID3D11ShaderResourceView> _worldPositionShaderResourceView;
	ComPtr<ID3D11ShaderResourceView> _lightingDiffuseShaderResourceView;
	ComPtr<ID3D11ShaderResourceView> _lightingSpecularShaderResourceView;
	ComPtr<ID3D11RenderTargetView> _albedoFrameBufferView;
	ComPtr<ID3D11RenderTargetView> _normalFrameBufferView;
	ComPtr<ID3D11RenderTargetView> _worldPositionFrameBufferView;
	ComPtr<ID3D11RenderTargetView> _lightingDiffuseFrameBufferView;
	ComPtr<ID3D11RenderTargetView> _lightingSpecularFrameBufferView;
#endif

	ComPtr<ID3D11Texture2D> _baseOutputTexture;
	ComPtr<ID3D11ShaderResourceView> _baseOutputShaderResourceView;
	ComPtr<ID3D11RenderTargetView> _baseOutputBufferView;

	ComPtr<ID3D11RenderTargetView> _frameBufferView;
	ComPtr<ID3D11Texture2D> _depthStencilBuffer;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;

	ComPtr<ID3D11Texture2D> _outputCopyTexture;
	ComPtr<ID3D11ShaderResourceView> _outputCopyShaderResourceView;

	ComPtr<ID3D11InputLayout> _inputLayout;
	D3D11_VIEWPORT _viewport;

	std::unique_ptr<ScreenQuad> _screenQuad = nullptr;

	std::chrono::time_point<std::chrono::steady_clock> _lastFrameTime = std::chrono::high_resolution_clock::now();
	double _timeSinceLastFixedUpdate = 0;
	std::weak_ptr<CameraComponent> _camera = {};

	int _selectedObject = -1;
	Vector3 _force{};
	Vector3 _forcePosition{};

	DirectX::XMFLOAT4 _colourEffect = { 1, 1, 1, 0 };
};

