#include "Structs/VS_SkyboxOut.hlsli"
#include "Samplers/S0_BilinearSampler.hlsli"

float4 PS_Main(VS_SkyboxOut input) : SV_TARGET
{
    return Skybox.Sample(BilinearSampler, input.TextureCoordinates);
}