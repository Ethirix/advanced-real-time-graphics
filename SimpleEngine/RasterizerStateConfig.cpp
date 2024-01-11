#include "RasterizerStateConfig.h"
#include <fstream>


RasterizerStateConfig::RasterizerStateConfig()
{
	std::ifstream fileStream("Assets/Configuration/RasterizerStateConfig.json");

	_json = nlohmann::json::parse(fileStream);

	//TODO: Finish RasterizerStateConfig
}