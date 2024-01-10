cbuffer CB0_ObjectCameraData : register(b0)
{
    float4x4 Projection;
    float4x4 View;
    float4x4 World;
    float4 CameraPosition;
}