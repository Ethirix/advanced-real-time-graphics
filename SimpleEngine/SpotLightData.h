#pragma once
#include <DirectXMath.h>

#include "SpotLightComponent.h"

struct SpotLightData
{
	SpotLightData() = default;

	explicit SpotLightData(const std::shared_ptr<SpotLightComponent>& spotLight) 
	{
		if (!spotLight->GameObject.expired())
		{
			DirectX::XMFLOAT3 pos = spotLight->GameObject.lock()->Transform->GetWorldPosition();
			Position = {pos.x, pos.y, pos.z, 1};
		}

		DiffuseColor = spotLight->DiffuseColor;
		DiffusePower = spotLight->DiffusePower;
		SpecularColor = spotLight->SpecularColor;
		SpecularPower = spotLight->SpecularPower;
		AmbientColor = spotLight->AmbientColor;

		ConstantAttenuation = spotLight->ConstantAttenuation;
		LinearAttenuation = spotLight->LinearAttenuation;
		QuadraticAttenuation = spotLight->QuadraticAttenuation;
		LightRadius = spotLight->LightRadius;

		InnerAngle = spotLight->InnerAngle;
		OuterAngle = spotLight->OuterAngle;
		Direction = spotLight->Direction;
	}

	DirectX::XMFLOAT4 Position {};
	DirectX::XMFLOAT3 DiffuseColor;
	float DiffusePower;
	DirectX::XMFLOAT3 SpecularColor;
	float SpecularPower;
	DirectX::XMFLOAT3 AmbientColor;
	float ConstantAttenuation;
	float LinearAttenuation;
	float QuadraticAttenuation;
	float LightRadius;

	float InnerAngle;
	float OuterAngle;
	DirectX::XMFLOAT3 Direction;
};