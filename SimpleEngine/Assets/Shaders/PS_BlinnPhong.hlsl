#include "Buffers/CB1_Material.hlsli"
#include "Buffers/CB2_Textures.hlsli"

#include "Functions/Lighting.hlsli"

#include "Structs/VS_BaseOut.hlsli"

#include "Structured Resources/T0_DiffuseTexture.hlsli"
#include "Structured Resources/T1_SpecularTexture.hlsli"
#include "Structured Resources/T2_NormalTexture.hlsli"

float4 PS_Main(VS_BaseOut input) : SV_TARGET
{
	input.TBNMatrix._m02_m12_m22 = normalize(input.TBNMatrix._m02_m12_m22);
	input.TBNMatrix._m01_m11_m21 = normalize(input.TBNMatrix._m01_m11_m21);
	input.TBNMatrix._m00_m10_m20 = normalize(input.TBNMatrix._m00_m10_m20);

	Textures textures = CreateTexturesFromTextures(T0_DiffuseTexture, HasDiffuseTexture, T1_SpecularTexture,
                                                   HasSpecularTexture, T2_NormalTexture, HasNormalTexture);
    Material material = CreateMaterial(DiffuseMaterial, AmbientMaterial, SpecularMaterial, SpecularExponent);

    LightingOut lighting = CalculateLighting(input.WorldPosition, input.TextureCoordinates, input.TBNMatrix, 
											 input.TextureCoordinates, textures, material);

	float4 color = float4(input.Color, 0);
	color += float4(lighting.AmbientOut, 0);
	color += float4(lighting.DiffuseOut, 0);
	color += float4(lighting.SpecularOut, 0);

	return color;
}
