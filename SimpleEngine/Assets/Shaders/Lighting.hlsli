#include "Samplers/S0_BilinearSampler.hlsli"
#include "Structs/LightData.hlsli"
#include "Structs/LightingOut.hlsli"
#include "Structs/Material.hlsli"
#include "Structs/Textures.hlsli"

LightingOut CalculatePointLight(
	LightData light,
	float3 fragmentPosition,
	float3 normal,
	float2 textureCoordinates,
	float3 cameraDirection,
	Textures textures,
	Material material)
{
    LightingOut lighting;
    lighting.DiffuseOut = float3(0, 0, 0);
    lighting.SpecularOut = float3(0, 0, 0);
    lighting.AmbientOut = float3(0, 0, 0);

    if (light.DiffusePower <= 0)
        return lighting;

    if (light.LightRadius <= 0 && light.LinearAttenuation <= 0 && light.QuadraticAttenuation <= 00)
    {
        float3 rayDirection = normalize(reflect(normalize(light.Position), normal));

        lighting.AmbientOut = light.AmbientColor;
        lighting.DiffuseOut = light.DiffuseColor * light.DiffusePower
    		* saturate(dot(normal, rayDirection));

        if (length(lighting.DiffuseOut > 0) && material.SpecularExponent > 0)
        {
            float3 halfVector = normalize(rayDirection + cameraDirection);

            lighting.SpecularOut = light.SpecularColor * light.SpecularPower
        		* pow(max(dot(halfVector, cameraDirection), 0.0f), material.SpecularExponent);
        }
    }
    else
    {
        float3 lightDir = normalize(light.Position - fragmentPosition);
        float lightDirDistance = length(lightDir);
        float lightDirDistSquared = lightDirDistance * lightDirDistance;
        float distance = length(light.Position - fragmentPosition);

        float attenuation = 1 /
			(light.ConstantAttenuation + abs(light.LinearAttenuation * distance) +
        pow(abs(light.QuadraticAttenuation), 2) * pow(distance, 2));

        if (light.LightRadius > 0)
        {
            attenuation *= clamp(1.0 - pow(distance, 2) / pow(light.LightRadius, 2.0), 0.0, 1.0);
        }

        float diffuseIntensity = saturate(dot(normal, lightDir));
        lighting.DiffuseOut = diffuseIntensity * light.DiffuseColor * light.DiffusePower / lightDirDistSquared;

        if (length(lighting.DiffuseOut > 0) && material.SpecularExponent > 0)
        {
            float3 halfVector = normalize(lightDir + cameraDirection);
            float specularIntensity = pow(saturate(dot(normal, halfVector)), material.SpecularExponent);
            lighting.SpecularOut = specularIntensity * light.SpecularColor * light.SpecularPower / lightDirDistSquared;
        }
	
        lighting.AmbientOut = light.AmbientColor;

        lighting.DiffuseOut = lighting.DiffuseOut * attenuation;
        lighting.SpecularOut = lighting.SpecularOut * attenuation;
        lighting.AmbientOut = lighting.AmbientOut * attenuation;
  
    }

    if (textures.Diffuse.HasTexture == true)
    {
        float4 textureColor = textures.Diffuse.Texture.Sample(BilinearSampler, textureCoordinates);
        lighting.AmbientOut *= textureColor;
        lighting.DiffuseOut *= textureColor;
    }
    else
    {
        lighting.AmbientOut *= material.Ambient;
        lighting.DiffuseOut *= material.Diffuse;
    }

    if (textures.Specular.HasTexture == true)
    {
        lighting.SpecularOut *= textures.Specular.Texture.Sample(BilinearSampler, textureCoordinates);
    }
    else
    {
        lighting.SpecularOut *= material.Specular;
    }

    return lighting;
}