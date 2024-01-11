#pragma once
#include <d3d11.h>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class RasterizerStateConfig
{
public:
	RasterizerStateConfig();
	std::vector<std::pair<std::string, D3D11_RASTERIZER_DESC>>
		RasterizerDescriptions;
	std::string DefaultRasterizerStateKey;
private:
	nlohmann::json _json;
};
