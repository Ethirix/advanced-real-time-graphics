#ifndef __CB2_TEXTURES_HLSLI__
#define __CB2_TEXTURES_HLSLI__

cbuffer CB2_Textures : register(b2)
{
    bool HasDiffuseTexture;
    bool HasSpecularTexture;
    int2 ___CB2Pad0;
}

#endif