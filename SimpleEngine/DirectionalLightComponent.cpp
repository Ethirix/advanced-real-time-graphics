#include "DirectionalLightComponent.h"

DirectionalLightComponent::DirectionalLightComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json)
	: LightComponent(owningGameObject)
{
	nlohmann::json diffuseColour = json["DiffuseColour"];
	nlohmann::json specularColour = json["SpecularColour"];
	nlohmann::json ambientColour = json["AmbientColour"];
	nlohmann::json direction = json["LightDirection"];

	Direction = DirectX::XMFLOAT4(direction["X"], direction["Y"], direction["Z"], 1);
	DiffuseColor = DirectX::XMFLOAT3(diffuseColour["R"], diffuseColour["G"], diffuseColour["B"]);
	DiffusePower = json["DiffusePower"];
	SpecularColor = DirectX::XMFLOAT3(specularColour["R"], specularColour["G"], specularColour["B"]);
	SpecularPower = json["SpecularPower"];
	AmbientColor = DirectX::XMFLOAT3(ambientColour["R"], ambientColour["G"], ambientColour["B"]);
}