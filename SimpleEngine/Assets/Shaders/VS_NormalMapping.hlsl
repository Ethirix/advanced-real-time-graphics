#include "Buffers/CB0_ObjectCameraData.hlsli"

#include "Functions/VectorSpaceConverter.hlsli"

#include "Structs/VS_BaseIn.hlsli"
#include "Structs/VS_NormalMappingOut.hlsli"

#include "Structured Resources/T9_PointLights.hlsli"

VS_NormalMappingOut VS_Main(VS_BaseIn input)
{
	VS_NormalMappingOut output = (VS_NormalMappingOut) 0;

    float4 pos4 = float4(input.Position, 1.0f);
    output.Position = mul(pos4, World);
    output.WorldPosition = output.Position;
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = float4(input.Normal, 0);
    output.TextureCoordinates = input.TextureCoordinates;
	output.WorldNormal = normalize(mul(float4(input.Normal, 0), World));

    float3 T = normalize(mul(input.Tangent.xyz, World));
    float3 B = normalize(mul(input.Bitangent.xyz, World));
    float3 N = output.WorldNormal;
    float3x3 inverseTBN = transpose(float3x3(T, B, N));

    output.TangentEye = ConvertVectorToSpace(Eye.xyz - output.WorldPosition, inverseTBN);
    output.TangentPointLight = ConvertVectorToSpace(T9_PointLights[0].Position.xyz - output.WorldPosition, inverseTBN);

    return output;
}
