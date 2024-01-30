#pragma once
#include <DirectXMath.h>

struct LightData
{
	LightData() = default;
	LightData(DirectX::XMFLOAT4 pos, 
		DirectX::XMFLOAT3 diffuseColour, float diffusePower, 
		DirectX::XMFLOAT3 specularColour, float specularPower,
		DirectX::XMFLOAT3 ambientColour, float constantAttenuation,
		float linearAttenuation, float quadraticAttenuation, 
		float lightRadius) : Position(pos), DiffuseColor(diffuseColour),
	DiffusePower(diffusePower), SpecularColor(specularColour), SpecularPower(specularPower),
	AmbientColor(ambientColour), ConstantAttenuation(constantAttenuation),
	LinearAttenuation(linearAttenuation), QuadraticAttenuation(quadraticAttenuation),
	LightRadius(lightRadius), ___LDPad0(0) {}

	DirectX::XMFLOAT4 Position;
	DirectX::XMFLOAT3 DiffuseColor;
	float DiffusePower;
	DirectX::XMFLOAT3 SpecularColor;
	float SpecularPower;
	DirectX::XMFLOAT3 AmbientColor;
	float ConstantAttenuation;
	float LinearAttenuation;
	float QuadraticAttenuation;
	float LightRadius;
	float ___LDPad0;
};