#ifndef __VS_BASEIN_HLSLI__
#define __VS_BASEIN_HLSLI__

struct VS_BaseIn
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TextureCoordinates : TEXCOORD;
    float3 Tangent : TANGENT;
    float3 Bitangent : BINORMAL;
};

#endif