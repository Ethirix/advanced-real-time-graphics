#include "Buffers/CB0_ObjectCameraData.hlsli"
#include "Buffers/CB1_Material.hlsli"
#include "Buffers/CB2_Textures.hlsli"
#include "Buffers/CB3_Lighting.hlsli"
#include "Functions/Lighting.hlsli"
#include "Structs/VS_BaseOut.hlsli"
#include "Structured Resources/T0_DiffuseTexture.hlsli"
#include "Structured Resources/T1_SpecularTexture.hlsli"
#include "Structured Resources/T2_NormalTexture.hlsli"
#include "Structured Resources/T8_Lighting.hlsli"

float4 PS_Main(VS_BaseOut input) : SV_TARGET
{
    input.WorldNormal = normalize(input.WorldNormal);
    float3 cameraDirection = normalize(CameraPosition.xyz - input.WorldPosition);

    float4 ambient = float4(0, 0, 0, 0);
    float4 diffuse = float4(0, 0, 0, 0);
    float4 specular = float4(0, 0, 0, 0);

    Textures textures = CreateTexturesFromTextures(T0_DiffuseTexture, HasDiffuseTexture, T1_SpecularTexture,
                                                   HasSpecularTexture, T2_NormalTexture, HasNormalTexture);
    Material material = CreateMaterial(DiffuseMaterial, AmbientMaterial, SpecularMaterial, SpecularExponent);

    //TODO: Convert Pixel Shader to use Tangent Space lighting.
    //Potentially pass a Boolean to the CalculatePointLight to determine what Space to use for calculations.
    for (uint i = 0; i < TotalLights; i++)
    {
        LightingOut light = CalculatePointLight(
	        T8_LightData[i],
	        input.WorldPosition,
	        input.Normal,
	        input.TextureCoordinates,
			input.Tangent,
            input.Bitangent,
            input.WorldNormal,
			cameraDirection,
			input.TangentEye,
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