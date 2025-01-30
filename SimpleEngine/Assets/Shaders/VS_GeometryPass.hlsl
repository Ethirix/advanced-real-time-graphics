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

    output.Normal = float4(input.Normal, 0);
    output.TextureCoordinates = input.TextureCoordinates;
    output.WorldNormal = normalize(mul(float4(input.Normal, 0), World));
    
    //MAKE TBN MATRIX
    //PASS TO PIXEL SHADER
    //CONVERT NORMAL MAPS TO WORLD SPACE WITH TBN MATRIX
    //INVERSED MATRIX IS TANGENT SPACE 

	return output;
}
