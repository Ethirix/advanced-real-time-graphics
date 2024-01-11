#pragma once
#include <d3d11.h>
#include <nlohmann/json.hpp>

class RasterizerStateConfig
{
public:
	RasterizerStateConfig();
	std::vector<D3D11_RASTERIZER_DESC> GetRasterizerDescriptions();

private:
	nlohmann::json _json;
	std::vector<D3D11_RASTERIZER_DESC> _rasterizerDescriptions;
};
