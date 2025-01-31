#ifndef __VS_BASEOUT_HLSLI__
#define __VS_BASEOUT_HLSLI__

struct VS_BaseOut
{
    float3 Color : COLOR;

    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION;

    float2 TextureCoordinates : TEXCOORD;

    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float3 Normal : NORMAL;
};

#endif