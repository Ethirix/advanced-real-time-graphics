#include "Samplers/S0_BilinearSampler.hlsli"
#include "Structs/VS_FinalPassOut.hlsli"
#include "Structured Resources/T24_FinalPass.hlsli"

float4 PS_Main(VS_FinalPassOut input) : SV_TARGET
{
    return float4(1 * input.Position.y, 0, 0, 1);

    return T24_FinalPass.Sample(S0_BilinearSampler, input.TextureCoordinates);
}
