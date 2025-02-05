#include "Buffers/CB1_Material.hlsli"
#include "Buffers/CB2_Textures.hlsli"

#include "Functions/Lighting.hlsli"

#include "Structs/VS_BaseOut.hlsli"

#include "Structured Resources/T0_DiffuseTexture.hlsli"
#include "Structured Resources/T1_SpecularTexture.hlsli"
#include "Structured Resources/T2_NormalTexture.hlsli"

float4 PS_Main(VS_BaseOut input) : SV_TARGET
{
	input.Normal = normalize(input.Normal);

	Textures textures = CreateTexturesFromTextures(T0_DiffuseTexture, HasDiffuseTexture, T1_SpecularTexture,
                                                   HasSpecularTexture, T2_NormalTexture, HasNormalTexture);
    Material material = CreateMaterial(DiffuseMaterial, AmbientMaterial, SpecularMaterial, SpecularExponent);

	float3 normal = CalculateNormal(input.TBNMatrix, input.TextureCoordinates, input.Normal, textures.Normal);
    LightingOut lighting = CalculateLighting(input.WorldPosition, normal, material);
    lighting = CalculateTextures(lighting, input.TextureCoordinates, textures, material);

	float4 color = float4(input.Color, 1);
	color += float4(lighting.AmbientOut, 1);
	color += float4(lighting.DiffuseOut, 1);
    color += float4(lighting.SpecularOut, 1);

	return color;
}
