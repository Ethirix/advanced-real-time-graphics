#include "PointLightComponent.h"

PointLightComponent::PointLightComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json)
	: LightComponent(owningGameObject)
{
	nlohmann::json diffuseColour = json["DiffuseColour"];
	nlohmann::json specularColour = json["SpecularColour"];
	nlohmann::json ambientColour = json["AmbientColour"];

	DiffuseColor = DirectX::XMFLOAT3(diffuseColour["R"], diffuseColour["G"], diffuseColour["B"]);
	DiffusePower = json["DiffusePower"];
	SpecularColor = DirectX::XMFLOAT3(specularColour["R"], specularColour["G"], specularColour["B"]);
	SpecularPower = json["SpecularPower"];
	AmbientColor = DirectX::XMFLOAT3(ambientColour["R"], ambientColour["G"], ambientColour["B"]);
	ConstantAttenuation = json["ConstantAttenuation"];
	LinearAttenuation = json["LinearAttenuation"];
	QuadraticAttenuation = json["QuadraticAttenuation"];
	LightRadius = json["LightRadius"];
}