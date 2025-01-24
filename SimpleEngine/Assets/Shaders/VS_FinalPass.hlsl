#include "Structs/VS_BaseIn.hlsli"
#include "Structs/VS_FinalPassOut.hlsli"

VS_FinalPassOut VS_Main(VS_BaseIn input)
{
    VS_FinalPassOut output = (VS_FinalPassOut) 0;

    output.Position = float4(input.Position, 1);
    output.Normal = input.Normal;
    output.TextureCoordinates = input.TextureCoordinates;

    return output;
}
