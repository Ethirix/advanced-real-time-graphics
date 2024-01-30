#ifndef __CB3_LIGHTING_HLSLI__
#define __CB3_LIGHTING_HLSLI__

#include "Structs/LightData.hlsli"

cbuffer CB3_Lighting : register(b3)
{
    LightData PointLights[32];
	uint ActiveLightCount;
    float3 ___CB3Pad0;
}

#endif