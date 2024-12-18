#include "Buffers/CB0_ObjectCameraData.hlsli"
#include "Structs/VS_BaseOut.hlsli"

VS_BaseOut VS_Main(
	float3 position : POSITION,
	float3 normal : NORMAL,
	float2 texCoords : TEXCOORDS,
	float3 tangent : TANGENT,
	float3 bitangent : BITANGENT)
{
    VS_BaseOut output = (VS_BaseOut) 0;

    float4 Pos4 = float4(position, 1.0f);
    output.Position = mul(Pos4, World);
    output.WorldPosition = output.Position;
    output.WorldNormal = normalize(mul(float4(normal, 0), World));
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.Normal = float4(normal, 0);
    output.TextureCoordinates = texCoords;

    return output;
}
