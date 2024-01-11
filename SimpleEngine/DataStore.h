#pragma once
#include <d3d11.h>

#include "ShaderComponent.h"
#include "Storage.h"

struct DataStore
{
	static Storage<ShaderData<ID3D11VertexShader>> VertexShaders;
	static Storage<ShaderData<ID3D11PixelShader>> PixelShaders;
	static Storage<ComPtr<ID3D11RasterizerState>> RasterizerStates;
};
