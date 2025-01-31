#include "Buffers/CB1_Material.hlsli"
#include "Buffers/CB2_Textures.hlsli"

#include "Functions/Lighting.hlsli"

#include "Structs/VS_BaseOut.hlsli"

#include "Structured Resources/T0_DiffuseTexture.hlsli"
#include "Structured Resources/T1_SpecularTexture.hlsli"
#include "Structured Resources/T2_NormalTexture.hlsli"

float4 PS_Main(VS_BaseOut input) : SV_TARGET
{
    float3x3 tbn = transpose(float3x3(normalize(input.Tangent), normalize(input.Bitangent), normalize(input.Normal)));

	Textures textures = CreateTexturesFromTextures(T0_DiffuseTexture, HasDiffuseTexture, T1_SpecularTexture,
                                                   HasSpecularTexture, T2_NormalTexture, HasNormalTexture);
    Material material = CreateMaterial(DiffuseMaterial, AmbientMaterial, SpecularMaterial, SpecularExponent);

    LightingOut lighting = CalculateLighting(input.WorldPosition, input.TextureCoordinates, tbn, 
											 input.TextureCoordinates, textures, material);

	float4 color = float4(input.Color, 0);
	color += float4(lighting.AmbientOut, 0);
	color += float4(lighting.DiffuseOut, 0);
	color += float4(lighting.SpecularOut, 0);

	return color;
}
