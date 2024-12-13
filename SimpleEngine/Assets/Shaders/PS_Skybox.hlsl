#include "Structs/VS_SkyboxOut.hlsli"
#include "Samplers/S0_BilinearSampler.hlsli"
#include "Structured Resources/T64_SkyboxTexture.hlsli"

float4 PS_Main(VS_SkyboxOut input) : SV_TARGET
{
    return T64_SkyboxTexture.Sample(S0_BilinearSampler, input.TextureCoordinates);
}