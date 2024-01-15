#include "Buffers/CB0_ObjectCameraData.hlsli"
#include "Buffers/CB1_Material.hlsli"
#include "Buffers/CB2_Textures.hlsli"
#include "Buffers/CB3_Lighting.hlsli"
#include "Functions/Lighting.hlsli"
#include "Structs/VS_BaseOut.hlsli"
#include "Structured Resources/T0_DiffuseTexture.hlsli"
#include "Structured Resources/T1_SpecularTexture.hlsli"

float4 PS_Main(VS_BaseOut input) : SV_TARGET
{
    input.WorldNormal = normalize(input.WorldNormal);
    float3 cameraDirection = normalize(CameraPosition - input.WorldPosition);

    float4 ambient = float4(0, 0, 0, 0);
    float4 diffuse = float4(0, 0, 0, 0);
    float4 specular = float4(0, 0, 0, 0);
    Textures textures = CreateTexturesFromTextures(DiffuseTexture, HasDiffuseTexture, SpecularTexture,
                                                   HasSpecularTexture);
    Material material = CreateMaterial(DiffuseMaterial, AmbientMaterial, SpecularMaterial, SpecularExponent);

    for (uint i = 0; i < ActiveLightCount; i++)
    {
        LightingOut light = CalculatePointLight(
	        PointLights[i], 
	        input.WorldPosition, 
	        input.WorldNormal,
	        input.TextureCoordinates,
			cameraDirection,
			textures,
			material
        );

        ambient += float4(light.AmbientOut, 0);
    	diffuse += float4(light.DiffuseOut, 0);
        specular += float4(light.SpecularOut, 0);
    }

	float4 color = float4(0, 0, 0, 0);
	color += ambient;
	color += diffuse;
	color += specular;

	return color;
}