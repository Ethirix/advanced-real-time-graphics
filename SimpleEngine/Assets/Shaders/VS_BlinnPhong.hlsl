#include "Buffers/CB0_ObjectCameraData.hlsli"

#include "Structs/VS_BaseIn.hlsli"
#include "Structs/VS_BaseOut.hlsli"

VS_BaseOut VS_Main(VS_BaseIn input)
{
    VS_BaseOut output = (VS_BaseOut) 0;

    float4 pos4 = float4(input.Position, 1.0f);
    output.Position = mul(pos4, World);
    output.WorldPosition = output.Position;
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.TextureCoordinates = input.TextureCoordinates;

    output.Tangent = normalize(mul(input.Tangent, World));
    output.Bitangent = normalize(mul(input.Bitangent, World));
    output.Normal = normalize(mul(float4(input.Normal, 0), World));

    return output;
}
