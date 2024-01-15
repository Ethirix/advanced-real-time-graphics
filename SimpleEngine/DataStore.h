#pragma once
#include <d3d11.h>

#include "ShaderData.h"
#include "Storage.h"

struct DataStore
{
	inline static Storage<ShaderData<ID3D11VertexShader>> VertexShaders = {};
	inline static Storage<ShaderData<ID3D11PixelShader>> PixelShaders = {};
	inline static Storage<Microsoft::WRL::ComPtr<ID3D11RasterizerState>> RasterizerStates = {};
	inline static Storage<Microsoft::WRL::ComPtr<ID3D11SamplerState>> SamplerStates = {};
	inline static Storage<Microsoft::WRL::ComPtr<ID3D11DepthStencilState>> DepthStencilStates = {};
};
