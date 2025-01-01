#ifndef __SPOTLIGHTDATA_HLSLI__
#define __SPOTLIGHTDATA_HLSLI__

struct SpotLightData
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

    float InnerAngle;
    float OuterAngle;
    float3 Direction;
};

#endif 