#ifndef __VS_BASEIN_HLSLI__
#define __VS_BASEIN_HLSLI__

struct VS_BaseIn
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TextureCoordinates : TEXCOORD;
    float4 Tangent : TANGENT;
    float4 Bitangent : BINORMAL;
};

#endif