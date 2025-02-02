#include "Structs/VS_BaseIn.hlsli"
#include "Structs/VS_BaseOut.hlsli"

VS_BaseOut VS_Main(VS_BaseIn input)
{
    VS_BaseOut output = (VS_BaseOut) 0;

    output.TextureCoordinates = input.TextureCoordinates;

    return output;
}
