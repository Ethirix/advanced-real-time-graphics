#include "Structs/VS_BaseOut.hlsli"

struct PSGeoPassOut
{
    float4 Albedo : SV_TARGET0;
    float4 Normal : SV_TARGET1;
};

PSGeoPassOut PS_Main(VS_BaseOut input)
{
    PSGeoPassOut output = (PSGeoPassOut) 0;

    return output;
}