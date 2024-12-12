#pragma once
#include <DirectXMath.h>

struct CBObjectCameraData
{
	DirectX::XMMATRIX Projection;
	DirectX::XMMATRIX View;
	DirectX::XMMATRIX World;
	DirectX::XMFLOAT4 CameraPosition;
};
