#ifndef __VS_FINALPASSOUT_HLSLI__
#define __VS_FINALPASSOUT_HLSLI__

struct VS_FinalPassOut
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TextureCoordinates : TEXCOORD;
};

#endif