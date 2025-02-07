#include "Functions/Lighting.hlsli"
#include "Structs/LightingOut.hlsli"
#include "Structs/VS_ScreenQuadOut.hlsli"

#include "Structured Resources/T17_DeferredNormalTexture.hlsli"
#include "Structured Resources/T18_DeferredWorldPositionTexture.hlsli"

struct PSLightPassOut
{
    float3 Diffuse : SV_TARGET0;
    float3 Specular : SV_TARGET1;
};

PSLightPassOut PS_Main(VS_ScreenQuadOut input) : SV_TARGET
{
    PSLightPassOut output = (PSLightPassOut) 0;

    float4 position = T18_DeferredWorldPositionTexture.Sample(S0_BilinearSampler, input.TextureCoordinates);
    float4 normal = T17_DeferredNormalTexture.Sample(S0_BilinearSampler, input.TextureCoordinates);
    float specularExponent = normal.a;

    LightingOut lighting = CalculateLighting(position.xyz, normalize(normal.rgb), specularExponent);

    output.Diffuse = lighting.DiffuseOut;
    output.Specular = lighting.SpecularOut;

    return output;
}
