#include "Buffers/CB1_Material.hlsli"
#include "Buffers/CB2_Textures.hlsli"

#include "Functions/Lighting.hlsli"

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
    float Depth   : SV_TARGET2; 
};

PSGeoPassOut PS_Main(VS_BaseOut input)
{
    PSGeoPassOut output = (PSGeoPassOut) 0;

    Textures textures = CreateTexturesFromTextures(T0_DiffuseTexture, HasDiffuseTexture, T1_SpecularTexture,
                                                   HasSpecularTexture, T2_NormalTexture, HasNormalTexture);
    Material material = CreateMaterial(DiffuseMaterial, AmbientMaterial, SpecularMaterial, SpecularExponent);

    output.Albedo = textures.Diffuse.HasTexture ? textures.Diffuse.Texture.Sample(S0_BilinearSampler, input.TextureCoordinates)
												: material.Diffuse;
	output.Normal.rgb = CalculateNormal(input.TBNMatrix, input.TextureCoordinates, input.Normal, textures.Normal);
    output.Normal.a = textures.Specular.HasTexture ? length(textures.Specular.Texture.Sample(S0_BilinearSampler, input.TextureCoordinates).rgb) / 3.0f
												   : length(material.Specular.rgb) / 3.0f;
    //output.Depth = (input.Position.w - NearZ) / (FarZ - NearZ);
    output.Depth = input.Position.z / input.Position.w;
    return output;
}