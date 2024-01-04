#pragma once
#include <memory>
#include <nlohmann/json.hpp>

#include "Config.h"

#define SP_C std::shared_ptr<Config>

class ConfigManager
{
public:
	static SP_C GetConfig();
	
private:
	inline static SP_C _instance = nullptr;

	static void ReadCameraConfig();
	static void ReadObjectManipulationConfig();
	static void ReadGameSceneConfig();

	static DirectionalLightInfo ProcessDirectionalLightConfig
		(const std::string& key, nlohmann::basic_json<>& values);
	static PointLightInfo ProcessPointLightConfig
		(const std::string& key, nlohmann::basic_json<>& values);
	static RenderObjectInfo ProcessRenderObjectConfig
		(const std::string& key, nlohmann::basic_json<>& values);
};