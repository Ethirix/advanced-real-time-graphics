#pragma once

#include <DirectXMath.h>

struct CBLighting
{
	unsigned MaxDirectionalLights;
    unsigned TotalDirectionalLights;
	
    unsigned MaxPointLights;
	unsigned TotalPointLights;

	unsigned MaxSpotLights;
	unsigned TotalSpotLights;

	DirectX::XMFLOAT2 ___CB3Pad0;
};
