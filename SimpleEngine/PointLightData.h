#pragma once
#include <DirectXMath.h>

#include "PointLightComponent.h"

struct PointLightData
{
	PointLightData() = default;

	explicit PointLightData(const std::shared_ptr<PointLightComponent>& pointLight) 
	{
		if (!pointLight->GameObject.expired())
		{
			DirectX::XMFLOAT3 pos = pointLight->GameObject.lock()->Transform->GetWorldPosition();
			Position = {pos.x, pos.y, pos.z, 1};
		}

		DiffuseColor = pointLight->DiffuseColor;
		DiffusePower = pointLight->DiffusePower;
		SpecularColor = pointLight->SpecularColor;
		SpecularPower = pointLight->SpecularPower;
		AmbientColor = pointLight->AmbientColor;

		ConstantAttenuation = pointLight->ConstantAttenuation;
		LinearAttenuation = pointLight->LinearAttenuation;
		QuadraticAttenuation = pointLight->QuadraticAttenuation;
		LightRadius = pointLight->LightRadius;
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
	float ___LDPad0{};
};