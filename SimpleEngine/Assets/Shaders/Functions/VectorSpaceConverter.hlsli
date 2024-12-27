#ifndef __VECTORSPACECONVERTER_HLSLI__
#define __VECTORSPACECONVERTER_HLSLI__

inline float3 ConvertVectorToSpace(float3 input, float3x3 invTBN)
{
    return mul(input, invTBN);
}

inline float4 ConvertVectorToSpace(float4 input, float4x4 invTBN)
{
    return mul(input, invTBN);
}

#endif