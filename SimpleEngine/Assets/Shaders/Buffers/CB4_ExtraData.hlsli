#ifndef __CB4_EXTRADATA_HLSLI__
#define __CB4_EXTRADATA_HLSLI__

cbuffer CB4_ExtraData : register(b4)
{
    float3 ColourEffect;
    uint ColourMode;

    uint BlurIterations;
	uint BlurSampleSize;

    float FocalDepth;
    float FocalBlendDistance;
}

#endif