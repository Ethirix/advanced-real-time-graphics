#include "Samplers/S0_BilinearSampler.hlsli"
#include "Structs/VS_ScreenQuadOut.hlsli"
#include "Structured Resources/T16_DeferredAlbedoTexture.hlsli"
#include "Structured Resources/T17_DeferredNormalTexture.hlsli"
#include "Structured Resources/T20_DeferredDiffuseLightingTexture.hlsli"
#include "Structured Resources/T21_DeferredSpecularLightingTexture.hlsli"

float4 PS_Main(VS_ScreenQuadOut input) : SV_TARGET
{
    float4 albedo = float4(T16_DeferredAlbedoTexture.Sample(S0_BilinearSampler, input.TextureCoordinates).rgb, 1);
    float4 diffuse = T20_DeferredDiffuseLightingTexture.Sample(S0_BilinearSampler, input.TextureCoordinates);
    float4 specular = float4(T21_DeferredSpecularLightingTexture.Sample(S0_BilinearSampler, input.TextureCoordinates).rgb, 1);
    float specularMult = T16_DeferredAlbedoTexture.Sample(S0_BilinearSampler, input.TextureCoordinates).a;

    float4 output = (albedo * diffuse) + (specular * specularMult);

    return output;
}
