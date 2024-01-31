#ifndef __CB2_TEXTURES_HLSLI__
#define __CB2_TEXTURES_HLSLI__

cbuffer CB2_Textures : register(b2)
{
    uint HasDiffuseTexture;
    uint HasSpecularTexture;
    float2 ___CB2Pad0;
}

#endif