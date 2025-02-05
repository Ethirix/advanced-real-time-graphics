#include "Buffers/CB1_Material.hlsli"
#include "Functions/Lighting.hlsli"
#include "Structs/LightingOut.hlsli"
#include "Structs/VS_ScreenQuadOut.hlsli"

#include "Structured Resources/T16_DeferredAlbedoTexture.hlsli"
#include "Structured Resources/T17_DeferredNormalTexture.hlsli"

struct PSLightPassOut
{
    float4 Diffuse : SV_TARGET0;
    float4 Specular : SV_TARGET1;
};

PSLightPassOut PS_Main(VS_ScreenQuadOut input) : SV_TARGET
{
    PSLightPassOut output = (PSLightPassOut) 0;

    Material material = CreateMaterial(DiffuseMaterial, AmbientMaterial, SpecularMaterial, SpecularExponent);

    float4 normal = T17_DeferredNormalTexture.Sample(S0_BilinearSampler, input.TextureCoordinates);
	LightingOut lighting = CalculateLighting(input.Position, normal.rgb, material);

    output.Diffuse = float4(lighting.DiffuseOut, 1);
    output.Specular = float4(lighting.SpecularOut * normal.a, 1);

    //output.Diffuse = float4(lighting.DiffuseOut, 1);
    //output.Specular = float4(lighting.SpecularOut, 1);

    return output;
}
