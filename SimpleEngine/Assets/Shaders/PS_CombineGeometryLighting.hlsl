#include "Samplers/S0_BilinearSampler.hlsli"
#include "Structs/VS_ScreenQuadOut.hlsli"
#include "Structured Resources/T16_DeferredAlbedoTexture.hlsli"
#include "Structured Resources/T17_DeferredNormalTexture.hlsli"
#include "Structured Resources/T20_DeferredDiffuseLightingTexture.hlsli"
#include "Structured Resources/T21_DeferredSpecularLightingTexture.hlsli"

float4 PS_Main(VS_ScreenQuadOut input) : SV_TARGET
{
    float4 diffuse = T16_DeferredAlbedoTexture.Sample(S0_BilinearSampler, input.TextureCoordinates)
					* T20_DeferredDiffuseLightingTexture.Sample(S0_BilinearSampler, input.TextureCoordinates);
    float4 specular = T21_DeferredSpecularLightingTexture.Sample(S0_BilinearSampler, input.TextureCoordinates)
					* T17_DeferredNormalTexture.Sample(S0_BilinearSampler, input.TextureCoordinates).a;
    float4 output = diffuse + specular;

    return output;
}
