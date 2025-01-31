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

	float3 T = normalize(mul(input.Tangent.xyz, World));
    float3 B = normalize(mul(input.Bitangent.xyz, World));
    float3 N = normalize(mul(float4(input.Normal, 0), World));
    output.TBNMatrix = transpose(float3x3(T, B, N));
    
    //MAKE TBN MATRIX
    //PASS TO PIXEL SHADER
    //CONVERT NORMAL MAPS TO WORLD SPACE WITH TBN MATRIX
    //INVERSED MATRIX IS TANGENT SPACE 

	return output;
}
