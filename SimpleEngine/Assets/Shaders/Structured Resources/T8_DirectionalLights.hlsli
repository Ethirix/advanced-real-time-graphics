#ifndef __T8_DIRECTIONLIGHTS_HLSLI__
#define __T8_DIRECTIONLIGHTS_HLSLI__

#include "Structs/DirectionalLightData.hlsli"

StructuredBuffer<DirectionalLightData> T8_DirectionalLights : register(t8);

#endif