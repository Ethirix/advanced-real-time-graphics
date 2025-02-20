#ifndef __VS_BASEOUT_HLSLI__
#define __VS_BASEOUT_HLSLI__

struct VS_BaseOut
{
    float3 Color : COLOR;

    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;

    float3 Normal : NORMAL;
    float2 TextureCoordinates : TEXCOORD;

    float3x3 TBNMatrix : TBNMATRIX;
};

#endif