#include "Samplers/S0_BilinearSampler.hlsli"
#include "Structs/VS_FinalPassOut.hlsli"
#include "Structured Resources/T24_FinalPass.hlsli"

float4 PS_Main(VS_FinalPassOut input) : SV_TARGET
{
    float depth = T24_FinalPass.Sample(S0_BilinearSampler, input.TextureCoordinates).r;
	float f = (2.0f * 0.1f) / (1000.0f + 0.1f - depth * (1000.0f - 0.1f));

    return float4(f, f, f, 1);
}
