#ifndef __LIGHTDATA_HLSLI__
#define __LIGHTDATA_HLSLI__

struct LightData
{
    float4 Position;
    float3 DiffuseColor;
    float DiffusePower;
    float3 SpecularColor;
    float SpecularPower;
    float3 AmbientColor;
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;
    float LightRadius;
    float ___LDPad0;
};

#endif 