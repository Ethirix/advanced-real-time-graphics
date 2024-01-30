#ifndef __CB1_MATERIALS_HLSLI__
#define __CB1_MATERIALS_HLSLI__

cbuffer CB1_Materials : register(b1)
{
    float4 DiffuseMaterial;
    float4 AmbientMaterial;
    float4 SpecularMaterial;
    float SpecularExponent;
    float3 ___CB1Pad0;
}

#endif