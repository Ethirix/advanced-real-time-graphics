#include "Buffers/CB1_Material.hlsli"
#include "Buffers/CB2_Textures.hlsli"

#include "Functions/Lighting.hlsli"

#include "Structs/LightingOut.hlsli"
#include "Structs/Material.hlsli"
#include "Structs/Textures.hlsli"
#include "Structs/VS_NormalMappingOut.hlsli"

#include "Structured Resources/T0_DiffuseTexture.hlsli"
#include "Structured Resources/T1_SpecularTexture.hlsli"
#include "Structured Resources/T2_NormalTexture.hlsli"

float4 PS_Main(VS_NormalMappingOut input) : SV_TARGET
{
    input.Normal = normalize(input.Normal);
    input.WorldNormal = normalize(input.WorldNormal);

    Textures textures = CreateTexturesFromTextures(T0_DiffuseTexture, HasDiffuseTexture, T1_SpecularTexture,
                                                   HasSpecularTexture, T2_NormalTexture, HasNormalTexture);
    Material material = CreateMaterial(DiffuseMaterial, AmbientMaterial, SpecularMaterial, SpecularExponent);

    LightingOut lighting = CalculateLighting(input.WorldPosition, input.TextureCoordinates, input.WorldNormal, input.TangentEye, input.TangentPointLight, textures, material);
    lighting = CalculateTextures(lighting, input.TextureCoordinates, textures, material);

    float4 color = float4(input.Color, 1);
    color += float4(lighting.AmbientOut, 1);
    color += float4(lighting.DiffuseOut, 1);
    color += float4(lighting.SpecularOut, 1);

	return color;
}