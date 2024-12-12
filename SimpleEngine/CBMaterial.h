#pragma once
#include <DirectXMath.h>

struct CBMaterial
{
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Specular;
	float SpecularExponent;

	DirectX::XMFLOAT3 ___CB1Pad0;
};
