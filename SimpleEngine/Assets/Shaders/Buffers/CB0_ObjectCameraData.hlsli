#ifndef __CB0_OBJECTCAMERADATA_HLSLI__
#define __CB0_OBJECTCAMERADATA_HLSLI__

cbuffer CB0_ObjectCameraData : register(b0)
{
    float4x4 Projection;
    float4x4 View;
    float4x4 World;
    float3 Eye;
    float NearZ;
    float3 At;
    float FarZ;
}

#endif