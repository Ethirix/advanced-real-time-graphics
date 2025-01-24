#include "Samplers/S0_BilinearSampler.hlsli"
#include "Structs/VS_FinalPassOut.hlsli"
#include "Structured Resources/T24_FinalPass.hlsli"

float4 PS_Main(VS_FinalPassOut input) : SV_TARGET
{
    return T24_FinalPass.Sample(S0_BilinearSampler, input.TextureCoordinates);
}
