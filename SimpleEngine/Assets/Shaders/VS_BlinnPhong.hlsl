#include "Buffers/CB0_ObjectCameraData.hlsli"
#include "Buffers/CB3_Lighting.hlsli"
#include "Functions/TangentSpaceConverter.hlsli"
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

    output.Normal = float4(input.Normal, 0);
    output.TextureCoordinates = input.TextureCoordinates;

    float4 t = normalize(mul(input.Tangent, World));
    float4 b = normalize(mul(input.Bitangent, World));
    float4 n = normalize(mul(float4(input.Normal, 0), World));
    float4x4 inverseTBN = transpose(float4x4(t, b, n, float4(0, 0, 0, 1)));

    //for (uint i = 0; i < ActiveLightCount; i++)
    //{
	   // PointLights[i].LightVertexPosition = VectorToTangentSpace(PointLights[i].Position - output.WorldPosition, inverseTBN);
    //}

    output.TangentEye = VectorToTangentSpace(CameraPosition - output.WorldPosition, inverseTBN);
    output.WorldNormal = n;

    return output;
}
