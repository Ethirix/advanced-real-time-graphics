#pragma once
#include <DirectXMath.h>

struct CBObjectCameraData
{
	DirectX::XMFLOAT4X4 Projection;
	DirectX::XMFLOAT4X4 View;
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4 CameraPosition;
};
