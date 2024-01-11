#include "RasterizerStateConfig.h"
#include <fstream>


RasterizerStateConfig::RasterizerStateConfig()
{
	std::ifstream fileStream("Assets/Configuration/RasterizerStateConfig.json");
	_json = nlohmann::json::parse(fileStream);

	DefaultRasterizerStateKey = _json["DefaultRasterizerKey"];

	for (auto rsConfig : _json["RasterizerStateDescriptions"])
	{
		std::string key;
		D3D11_RASTERIZER_DESC	description = {};

		description.FillMode = static_cast<D3D11_FILL_MODE>(rsConfig["FillMode"]);
		description.CullMode = static_cast<D3D11_CULL_MODE>(rsConfig["CullMode"]);
		description.FrontCounterClockwise = rsConfig["FrontCounterClockwise"];
		description.DepthBias = rsConfig["DepthBias"];
		description.SlopeScaledDepthBias = rsConfig["SlopeScaledDepthBias"];
		description.DepthBiasClamp = rsConfig["DepthBiasClamp"];
		description.DepthClipEnable = rsConfig["DepthClipEnable"];
		description.ScissorEnable = rsConfig["ScissorEnable"];
		description.MultisampleEnable = rsConfig["MultisampleEnable"];
		description.AntialiasedLineEnable = rsConfig["AntialiasedLineEnable"];

		RasterizerDescriptions.emplace_back(key, description);
	}
}