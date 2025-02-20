#include "Buffers/CB0_ObjectCameraData.hlsli"
#include "Structs/VS_BaseIn.hlsli"
#include "Structs/VS_BaseOut.hlsli"

VS_BaseOut VS_Main(VS_BaseIn input)
{
    VS_BaseOut output = (VS_BaseOut) 0;

    float4 pos4 = float4(input.Position, 1.0f);
    output.Position = mul(pos4, World);
    output.WorldPosition = output.Position;
    output.Position = mul(output.Position, ViewProjection);
    output.TextureCoordinates = input.TextureCoordinates;
	output.Normal = normalize(mul(input.Normal, (float3x3)World));

    float3 t = normalize(mul(input.Tangent, (float3x3)World));
    float3 b = normalize(mul(input.Bitangent, (float3x3)World));
    float3 n = output.Normal;
    output.TBNMatrix = float3x3(t, b, n);

	return output;
}
