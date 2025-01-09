#pragma once
#include <DirectXMath.h>

#include "DirectionalLightComponent.h"

struct DirectionalLightData
{
	DirectionalLightData() = default;

	explicit DirectionalLightData(const std::shared_ptr<DirectionalLightComponent>& directionalLight) 
	{
		Direction = directionalLight->Direction;
		DiffuseColor = directionalLight->DiffuseColor;
		DiffusePower = directionalLight->DiffusePower;
		SpecularColor = directionalLight->SpecularColor;
		SpecularPower = directionalLight->SpecularPower;
		AmbientColor = directionalLight->AmbientColor;
	}

	DirectX::XMFLOAT4 Direction;
	DirectX::XMFLOAT3 DiffuseColor;
	float DiffusePower;
	DirectX::XMFLOAT3 SpecularColor;
	float SpecularPower;
	DirectX::XMFLOAT3 AmbientColor;

	float ___LDPad0{};
};