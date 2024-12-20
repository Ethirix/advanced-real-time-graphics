#ifndef __VS_BASEOUT_HLSLI__
#define __VS_BASEOUT_HLSLI__

struct VS_BaseOut
{
    float4 Color : COLOR;

    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;

    float4 WorldNormal : WORLDNORMAL;
    float4 WorldPosition : POSITION;

    float2 TextureCoordinates : TEXCOORD;

    float4 TangentEye : TANGENTEYE;

    float3x3 InverseTBN : TBN;
	//float4 LightTangentPosition : LIGHTTANGENT;
};

#endif