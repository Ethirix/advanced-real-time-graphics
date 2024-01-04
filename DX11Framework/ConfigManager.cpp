#include "ConfigManager.h"
#include <nlohmann/json.hpp>
#include <fstream>

std::shared_ptr<Config> ConfigManager::GetConfig()
{
	if (!_instance)
	{
		_instance = std::make_shared<Config>(Config());

		ReadCameraConfig();
		ReadObjectManipulationConfig();
		ReadGameSceneConfig();
	}

	return _instance;
}

void ConfigManager::ReadCameraConfig()
{
	std::ifstream fStream("Config/CameraConfig.json");
	nlohmann::json cameraData = nlohmann::json::parse(fStream);
	_instance->CameraConfig.FieldOfView = cameraData["FieldOfView"];
	_instance->CameraConfig.MovementMultiplier = cameraData["MovementMultiplier"];
	_instance->CameraConfig.RotationMultiplier = cameraData["RotationMultiplier"];
	_instance->CameraConfig.NearPlane = cameraData["NearPlane"];
	_instance->CameraConfig.FarPlane = cameraData["FarPlane"];
	fStream.close();
}

void ConfigManager::ReadObjectManipulationConfig()
{
	std::ifstream fStream("Config/ObjectManipulationConfig.json");
	nlohmann::json objectManipulationData = nlohmann::json::parse(fStream);
	_instance->ObjectManipulationConfig.MovementMultiplier = objectManipulationData["MovementMultiplier"];
	_instance->ObjectManipulationConfig.RotationMultiplier = objectManipulationData["RotationMultiplier"];
	fStream.close();
}

void ConfigManager::ReadGameSceneConfig()
{
	std::ifstream fStream("Config/GameScene.json");
	nlohmann::json gameSceneData = nlohmann::json::parse(fStream);

	for (auto& [objectKey, objectValues] : gameSceneData.items())
	{
		if (objectValues["Type"] == "DirectionalLight")
		{
			ProcessDirectionalLightConfig(objectKey, objectValues);
		}
		else if (objectValues["Type"] == "PointLight")
		{

		}
	}

	fStream.close();
}

DirectionalLightInfo ConfigManager::ProcessDirectionalLightConfig
	(const std::string& key, nlohmann::basic_json<>& values)
{
	DirectionalLightInfo info = {};
	info.Name = key;

	info.LightPosition = {
		values["LightPosition"]["x"],
		values["LightPosition"]["y"],
		values["LightPosition"]["z"]
	};

	info.DiffuseColor = {
		values["DiffuseColor"]["x"],
		values["DiffuseColor"]["y"],
		values["DiffuseColor"]["z"]
	};

	info.DiffusePower = values["DiffusePower"];

	info.SpecularColor = {
		values["SpecularColor"]["x"],
		values["SpecularColor"]["y"],
		values["SpecularColor"]["z"]
	};

	info.SpecularPower = values["SpecularPower"];

	info.AmbientColor = {
		values["AmbientColor"]["x"],
		values["AmbientColor"]["y"],
		values["AmbientColor"]["z"]
	};

	return info;
}

PointLightInfo ConfigManager::ProcessPointLightConfig
	(const std::string& key, nlohmann::basic_json<>& values)
{
	return {};
}

RenderObjectInfo ConfigManager::ProcessRenderObjectConfig
	(const std::string& key, nlohmann::basic_json<>& values)
{
	return{};
}