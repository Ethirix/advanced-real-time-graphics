#include "Buffers/CB0_ObjectCameraData.hlsli"
#include "Structs/VS_SkyboxOut.hlsli"

VS_SkyboxOut VS_Main(
	float3 Position : POSITION,
	float3 NormalVector : NORMAL,
	float2 TexCoords : TEXCOORDS
)
{
    VS_SkyboxOut output = (VS_SkyboxOut) 0;
    float4 pos4 = float4(Position, 1);
    output.Position = mul(pos4, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.Position = output.Position.xyww;
    output.TextureCoordinates = Position;

    return output;
}