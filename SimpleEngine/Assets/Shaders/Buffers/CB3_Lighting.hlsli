#include "../Structs/LightData.hlsli"

cbuffer LightBuffer : register(b1)
{
    LightData PointLights[32];
	uint ActiveLightCount;
    float3 ___CB3Pad0;
}
