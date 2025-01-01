#ifndef __CB3_LIGHTING_HLSLI__
#define __CB3_LIGHTING_HLSLI__

cbuffer CB3_Lighting : register(b3)
{
    uint MaxDirectionalLights;
    uint TotalDirectionalLights;

    uint MaxPointLights;
	uint TotalPointLights;

	uint MaxSpotLights;
	uint TotalSpotLights;

    int2 ___CB3Pad0;
}

#endif