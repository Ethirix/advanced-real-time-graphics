#pragma once
#include <DirectXMath.h>

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TextureCoordinate;
	DirectX::XMFLOAT4 Tangent;
	DirectX::XMFLOAT4 Bitangent;
};
