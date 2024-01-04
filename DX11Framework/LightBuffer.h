#pragma once
#include <DirectXMath.h>

#include "Buffer.h"

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
	float ___Pad2;
};


struct LightBuffer : Buffer
{
	unsigned ActiveLightCount;
	DirectX::XMFLOAT3 ___Pad1;
	LightData PointLights[32];
};