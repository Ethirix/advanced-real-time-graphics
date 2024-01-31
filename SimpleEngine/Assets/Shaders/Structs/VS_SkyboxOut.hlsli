#ifndef __VS_SKYBOXOUT_HLSLI__
#define __VS_SKYBOXOUT_HLSLI__

struct VS_SkyboxOut
{
    float4 Position : SV_POSITION;
    float3 TextureCoordinates : TEXCOORD;
};

#endif