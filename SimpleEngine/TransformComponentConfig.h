#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>

struct TransformComponentConfig
{
	std::string Name;
	std::vector<TransformComponentConfig> Children;
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Scale;
	DirectX::XMFLOAT3 Rotation;
};
