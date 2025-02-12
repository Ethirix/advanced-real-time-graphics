#pragma once
#include <DirectXMath.h>

struct CBObjectCameraData
{
	DirectX::XMMATRIX ViewProjection;
	DirectX::XMMATRIX InverseViewProjection;
	DirectX::XMMATRIX World;
	DirectX::XMFLOAT3 Eye;
	float NearZ;
	DirectX::XMFLOAT3 At;
	float FarZ;
};
