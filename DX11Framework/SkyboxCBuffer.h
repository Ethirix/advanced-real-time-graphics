#pragma once
#include <DirectXMath.h>

struct SkyboxCBuffer
{
	DirectX::XMMATRIX Projection, View, World;
};