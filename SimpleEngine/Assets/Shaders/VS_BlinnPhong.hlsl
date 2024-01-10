#include "Buffers/CB0_Matrix.hlsli"
#include "VS_BaseOut.hlsli"

VS_BaseOut VS_Main(
	float3 Position : POSITION,
	float3 NormalVector : NORMAL,
	float2 TexCoords : TEXCOORDS)
{
    VS_BaseOut output = (VS_BaseOut) 0;

    float4 Pos4 = float4(Position, 1.0f);
    output.Position = mul(Pos4, World);
    output.WorldPosition = output.Position;
    output.WorldNormal = normalize(mul(float4(NormalVector, 0), World));
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.Normal = (NormalVector, 0);
    output.TextureCoordinates = TexCoords;

    return output;
}