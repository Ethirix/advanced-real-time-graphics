#include "ConfigManager.h"
#include <nlohmann/json.hpp>
#include <fstream>

std::shared_ptr<Config> ConfigManager::GetConfig()
{
	if (!_instance)
	{
		_instance = std::make_shared<Config>(Config());

		std::ifstream fStream("Config/CameraConfig.json");
		nlohmann::json cameraData = nlohmann::json::parse(fStream);
		_instance->CameraConfig.FieldOfView = cameraData["FieldOfView"];
		_instance->CameraConfig.MovementMultiplier = cameraData["MovementMultiplier"];
		_instance->CameraConfig.RotationMultiplier = cameraData["RotationMultiplier"];
		_instance->CameraConfig.NearPlane = cameraData["NearPlane"];
		_instance->CameraConfig.FarPlane = cameraData["FarPlane"];
		fStream.close();

		fStream.open("Config/ObjectManipulationConfig.json");
		nlohmann::json objectManipulationData = nlohmann::json::parse(fStream);
		_instance->ObjectManipulationConfig.MovementMultiplier = objectManipulationData["MovementMultiplier"];
		_instance->ObjectManipulationConfig.RotationMultiplier = objectManipulationData["RotationMultiplier"];
		fStream.close();
	}

	return _instance;
}