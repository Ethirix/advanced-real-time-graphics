#include "Structs/VS_BaseIn.hlsli"
#include "Structs/VS_ScreenQuadOut.hlsli"

VS_ScreenQuadOut VS_Main(VS_BaseIn input)
{
    VS_ScreenQuadOut output = (VS_ScreenQuadOut) 0;

    output.Position = float4(input.Position, 1);
    output.TextureCoordinates = input.TextureCoordinates;

    return output;
}
