#include "Buffers/CB0_ObjectCameraData.hlsli"
#include "Buffers/CB3_Lighting.hlsli"
#include "Functions/TangentSpaceConverter.hlsli"
#include "Structs/VS_BaseIn.hlsli"
#include "Structs/VS_BaseOut.hlsli"
#include "Structured Resources/T8_Lighting.hlsli"

VS_BaseOut VS_Main(VS_BaseIn input)
{
    VS_BaseOut output = (VS_BaseOut) 0;

    //http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/

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
    output.Tangent = t;
    output.Bitangent = b;
	output.WorldNormal = n;

    float3x3 inverseTBN = transpose(float3x3(t.xyz, b.xyz, n.xyz));
    output.TangentEye = float4(VectorToTangentSpace(CameraPosition - output.WorldPosition, inverseTBN), 0);

    return output;
}
