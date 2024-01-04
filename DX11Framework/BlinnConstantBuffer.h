#pragma once
#include <DirectXMath.h>

#include "Buffer.h"

struct BlinnConstantBuffer : Buffer
{
	DirectX::XMMATRIX Projection, View, World;
	DirectX::XMFLOAT4 AmbientMaterial;
	DirectX::XMFLOAT4 DiffuseMaterial;
	unsigned HasDiffuseTexture;
	unsigned HasSpecularTexture;
	DirectX::XMFLOAT2 ___Pad0;
	DirectX::XMFLOAT4 SpecularMaterial;
	float SpecularExponent;
	DirectX::XMFLOAT3 CameraPosition;
};
