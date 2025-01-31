#include "Buffers/CB1_Material.hlsli"
#include "Buffers/CB2_Textures.hlsli"
#include "Samplers/S0_BilinearSampler.hlsli"
#include "Structs/Material.hlsli"
#include "Structs/Textures.hlsli"
#include "Structs/VS_BaseOut.hlsli"
#include "Structured Resources/T0_DiffuseTexture.hlsli"
#include "Structured Resources/T1_SpecularTexture.hlsli"
#include "Structured Resources/T2_NormalTexture.hlsli"

struct PSGeoPassOut
{
    float4 Albedo : SV_TARGET0;
    float4 Normal : SV_TARGET1; //rgb - normal a - specular
};

PSGeoPassOut PS_Main(VS_BaseOut input)
{
    PSGeoPassOut output = (PSGeoPassOut) 0;

    Textures textures = CreateTexturesFromTextures(T0_DiffuseTexture, HasDiffuseTexture, T1_SpecularTexture,
                                                   HasSpecularTexture, T2_NormalTexture, HasNormalTexture);
    Material material = CreateMaterial(DiffuseMaterial, AmbientMaterial, SpecularMaterial, SpecularExponent);

    if (textures.Diffuse.HasTexture)
    {
        output.Albedo = textures.Diffuse.Texture.Sample(S0_BilinearSampler, input.TextureCoordinates);
    }
	else
	{
        output.Albedo = material.Diffuse;
    }

    if (textures.Normal.HasTexture)
    {
        output.Normal.rgb = normalize(textures.Normal.Texture.Sample(S0_BilinearSampler, input.TextureCoordinates).xyz);
    }
	else
	{
        output.Normal.rgb = normalize(float3(input.TBNMatrix._m02_m12_m22));
    }

    if (textures.Specular.HasTexture)
    {
        output.Normal.a = length(textures.Specular.Texture.Sample(S0_BilinearSampler, input.TextureCoordinates).rgb) / 3.0f;
    }
    else
    {
        output.Normal.a = length(material.Specular.rgb) / 3.0f;
    }

    return output;
}