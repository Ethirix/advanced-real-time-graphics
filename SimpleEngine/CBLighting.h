#pragma once
#include "LightData.h"

struct CBLighting
{
	LightData PointLights[32];
	unsigned ActiveLightCount;
	DirectX::XMFLOAT3 ___CB3Pad0;
};
