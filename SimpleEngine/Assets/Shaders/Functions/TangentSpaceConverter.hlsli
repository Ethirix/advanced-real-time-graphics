#ifndef __TANGENTSPACECONVERTER_HLSLI__
#define __TANGENTSPACECONVERTER_HLSLI__

float3 VectorToTangentSpace(float3 input, float3x3 invTBN)
{
    return normalize(mul(input, invTBN));
}

#endif