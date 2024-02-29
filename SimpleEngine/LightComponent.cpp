#include "LightComponent.h"
#include "GameObject.h"

LightComponent::LightComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json) : ComponentBase(owningGameObject)
{
	nlohmann::json diffuseColour = json["DiffuseColour"];
	nlohmann::json specularColour = json["SpecularColour"];
	nlohmann::json ambientColour = json["AmbientColour"];
	DirectX::XMFLOAT3 pos = GameObject.lock()->Transform->GetPosition();

	Light = LightData(
		DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 1),
		DirectX::XMFLOAT3(diffuseColour["R"], diffuseColour["G"], diffuseColour["B"]),
		json["DiffusePower"],
		DirectX::XMFLOAT3(specularColour["R"], specularColour["G"], specularColour["B"]),
		json["SpecularPower"],
		DirectX::XMFLOAT3(ambientColour["R"], ambientColour["G"], ambientColour["B"]),
		json["ConstantAttenuation"],
		json["LinearAttenuation"],
		json["QuadraticAttenuation"],
		json["LightRadius"]
	);
}


void LightComponent::Update(double deltaTime)
{
	auto position = GameObject.lock()->Transform->GetPosition();
	Light.Position = { position.x, position.y, position.z, 1 };
}
