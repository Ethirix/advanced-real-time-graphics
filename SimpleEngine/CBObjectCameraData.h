#pragma once
#include <DirectXMath.h>

struct CBObjectCameraData
{
	DirectX::XMMATRIX Projection;
	DirectX::XMMATRIX View;
	DirectX::XMMATRIX InverseProjection;
	DirectX::XMMATRIX InverseView;
	DirectX::XMMATRIX World;
	DirectX::XMFLOAT3 Eye;
	float NearZ;
	DirectX::XMFLOAT3 At;
	float FarZ;
};
