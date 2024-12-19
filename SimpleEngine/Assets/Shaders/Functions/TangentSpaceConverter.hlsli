#ifndef __TANGENTSPACECONVERTER_HLSLI__
#define __TANGENTSPACECONVERTER_HLSLI__

float4 VectorToTangentSpace(float4 input, float4x4 invTBN)
{
    return normalize(mul(input, invTBN));
}

#endif