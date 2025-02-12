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
    float4 Albedo        : SV_TARGET0; //rgb - albedo a - specular
    float4 Normal        : SV_TARGET1; //rgb - normal a - specularExpo
    float4 WorldPosition : SV_TARGET2;
    float  Depth         : SV_TARGET3;
};

PSGeoPassOut PS_Main(VS_BaseOut input)
{
    PSGeoPassOut output = (PSGeoPassOut) 0;

    Textures textures = CreateTexturesFromTextures(T0_DiffuseTexture, HasDiffuseTexture, T1_SpecularTexture,
                                                   HasSpecularTexture, T2_NormalTexture, HasNormalTexture);
    Material material = CreateMaterial(DiffuseMaterial, AmbientMaterial, SpecularMaterial, SpecularExponent);

    float3 spec = textures.Specular.HasTexture ? textures.Specular.Texture.Sample(S0_BilinearSampler, input.TextureCoordinates).rgb
												   : material.Specular.rgb;

    output.Albedo.rgb = textures.Diffuse.HasTexture ? textures.Diffuse.Texture.Sample(S0_BilinearSampler, input.TextureCoordinates)
												: material.Diffuse;
    output.Albedo.a = (spec.r + spec.g + spec.b) / 3.0f;
    
	output.Normal.rgb = CalculateNormal(input.TBNMatrix, input.TextureCoordinates, input.Normal, textures.Normal);
    output.Normal.a = SpecularExponent;

    output.WorldPosition = input.WorldPosition;

    output.Depth = (input.Position.w - NearZ) / (FarZ - NearZ);

    return output;
}