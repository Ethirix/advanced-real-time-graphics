#ifndef __VS_BASEOUT_HLSLI__
#define __VS_BASEOUT_HLSLI__

struct VS_BaseOut
{
    float4 Color : COLOR;

    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION0;

    float4 Normal : NORMAL0;
    float4 WorldNormal : WORLDNORMAL0;

    float2 TextureCoordinates : TEXCOORDS0;
};

#endif