#ifndef __DIRECTINOALLIGHTDATA_HLSLI__
#define __DIRECTINOALLIGHTDATA_HLSLI__

struct DirectionalLightData
{
    float4 Direction;

    float3 DiffuseColor;
    float DiffusePower;
    float3 SpecularColor;
    float SpecularPower;
    float3 AmbientColor;

    float ___LDPad0;
};

#endif 