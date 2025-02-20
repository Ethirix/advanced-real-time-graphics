#include "Buffers/CB0_ObjectCameraData.hlsli"
#include "Structs/VS_BaseIn.hlsli"
#include "Structs/VS_SkyboxOut.hlsli"

VS_SkyboxOut VS_Main(VS_BaseIn input)
{
    VS_SkyboxOut output = (VS_SkyboxOut) 0;
    float4 pos4 = float4(input.Position, 1);
    output.Position = mul(pos4, World);
    output.Position = mul(output.Position, ViewProjection);
    output.Position = output.Position.xyww;
    output.TextureCoordinates = input.Position;

    return output;
}