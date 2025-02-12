#ifndef __VS_SCREENQUADOUT_HLSLI__
#define __VS_SCREENQUADOUT_HLSLI__

struct VS_ScreenQuadOut
{
    float4 Position : SV_POSITION;
    float2 TextureCoordinates : TEXCOORD;
};

#endif