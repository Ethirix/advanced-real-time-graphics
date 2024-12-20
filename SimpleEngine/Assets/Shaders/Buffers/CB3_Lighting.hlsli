#ifndef __CB3_LIGHTING_HLSLI__
#define __CB3_LIGHTING_HLSLI__

cbuffer CB3_Lighting : register(b3)
{
    uint MaxLights;
	uint TotalLights;
    float2 ___CB3Pad0;
}

#endif