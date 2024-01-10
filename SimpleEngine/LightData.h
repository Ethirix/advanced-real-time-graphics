#pragma once
#include <DirectXMath.h>

struct LightData
{
	DirectX::XMFLOAT4 Position;
	DirectX::XMFLOAT3 DiffuseColor;
	float DiffusePower;
	DirectX::XMFLOAT3 SpecularColor;
	float SpecularPower;
	DirectX::XMFLOAT3 AmbientColor;
	float ConstantAttenuation = 1;
	float LinearAttenuation;
	float QuadraticAttenuation;
	float LightRadius;
	float ___LDPad0;
};