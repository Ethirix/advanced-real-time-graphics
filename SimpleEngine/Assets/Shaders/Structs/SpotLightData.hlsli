#ifndef __SpotLIGHTDATA_HLSLI__
#define __SpotLIGHTDATA_HLSLI__

struct SpotLightData
{
    float4 Position;

    float3 DiffuseColor;
    float DiffusePower;
    float3 SpecularColor;
    float SpecularPower;
    float3 AmbientColor;

    float InnerAngle;
    float OuterAngle;
    float3 Direction;
};

#endif 