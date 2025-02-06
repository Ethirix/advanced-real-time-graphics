#include "Samplers/S0_BilinearSampler.hlsli"

#include "Structs/VS_ScreenQuadOut.hlsli"

#include "Structured Resources/T16_DeferredAlbedoTexture.hlsli"
#include "Structured Resources/T20_DeferredDiffuseLightingTexture.hlsli"
#include "Structured Resources/T21_DeferredSpecularLightingTexture.hlsli"

float4 PS_Main(VS_ScreenQuadOut input) : SV_TARGET
{
    float3 albedo = T16_DeferredAlbedoTexture.Sample(S0_BilinearSampler, input.TextureCoordinates).rgb;
    float3 diffuse = T20_DeferredDiffuseLightingTexture.Sample(S0_BilinearSampler, input.TextureCoordinates).rgb;
    float3 specular = T21_DeferredSpecularLightingTexture.Sample(S0_BilinearSampler, input.TextureCoordinates).rgb;
    float specularMap = T16_DeferredAlbedoTexture.Sample(S0_BilinearSampler, input.TextureCoordinates).a;

    float4 output = float4(albedo * diffuse, 1);
    output += float4(specular * specularMap, 1);

    return output;
}
