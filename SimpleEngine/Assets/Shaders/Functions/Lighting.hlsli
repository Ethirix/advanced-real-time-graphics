#ifndef __LIGHTING_HLSLI__
#define __LIGHTING_HLSLI__

#include "Functions/TangentSpaceConverter.hlsli"
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
	float3 tangent,
	float3 bitangent,
	float3 worldNormal,
	float3 cameraDirection,
	float3 cameraTangentDirection,
	Textures textures,
	Material material)
	
{
    LightingOut lighting;
    lighting.DiffuseOut = float3(0, 0, 0);
    lighting.SpecularOut = float3(0, 0, 0);
    lighting.AmbientOut = float3(0, 0, 0);

    if (light.DiffusePower <= 0)
        return lighting;

    float3 calculatedNormal = worldNormal;
    float3 lightPosition = light.Position;

    if (textures.Normal.HasTexture)
    {
        float4 normalMap = textures.Normal.Texture.Sample(S0_BilinearSampler, textureCoordinates);
        normalMap = (normalMap * 2.0f) - 1.0f;
        calculatedNormal = normalize((normalMap.x * tangent) + (normalMap.y * bitangent) + (normalMap.z * worldNormal));

        lightPosition = VectorToTangentSpace(light.Position - fragmentPosition, transpose(float3x3(tangent, bitangent, calculatedNormal)));
    }

    if (light.LightRadius <= 0 && light.LinearAttenuation <= 0 && light.QuadraticAttenuation <= 0)
    {
        float3 rayDirection = normalize(reflect(normalize(lightPosition), calculatedNormal));

        lighting.AmbientOut = light.AmbientColor;
        lighting.DiffuseOut = light.DiffuseColor * light.DiffusePower
    		* saturate(dot(calculatedNormal, rayDirection));

        if (length(lighting.DiffuseOut > 0) && material.SpecularExponent > 0)
        {
            float3 halfVector = normalize(rayDirection + cameraTangentDirection);

            lighting.SpecularOut = light.SpecularColor * light.SpecularPower
        		* pow(max(dot(halfVector, cameraTangentDirection), 0.0f), material.SpecularExponent);
        }
    }
    else
    {
        float3 lightDir = normalize(lightPosition - fragmentPosition);
        float lightDirDistance = length(lightDir);
        float lightDirDistSquared = lightDirDistance * lightDirDistance;
        float distance = length(lightPosition - fragmentPosition);

        float attenuation = 1 /
			(light.ConstantAttenuation + abs(light.LinearAttenuation * distance) +
        pow(abs(light.QuadraticAttenuation), 2) * pow(distance, 2));

        if (light.LightRadius > 0)
        {
            attenuation *= clamp(1.0 - pow(distance, 2) / pow(light.LightRadius, 2.0), 0.0, 1.0);
        }

        float diffuseIntensity = saturate(dot(calculatedNormal, lightDir));
        lighting.DiffuseOut = diffuseIntensity * light.DiffuseColor * light.DiffusePower / lightDirDistSquared;

        if (length(lighting.DiffuseOut > 0) && material.SpecularExponent > 0)
        {
            float3 halfVector = normalize(lightDir + cameraTangentDirection);
            float specularIntensity = pow(saturate(dot(calculatedNormal, halfVector)), material.SpecularExponent);
            lighting.SpecularOut = specularIntensity * light.SpecularColor * light.SpecularPower / lightDirDistSquared;
        }

        lighting.DiffuseOut = lighting.DiffuseOut * attenuation;
        lighting.SpecularOut = lighting.SpecularOut * attenuation;
        lighting.AmbientOut = light.AmbientColor * attenuation;
    }

    if (textures.Diffuse.HasTexture)
    {
        float4 textureColor = textures.Diffuse.Texture.Sample(S0_BilinearSampler, textureCoordinates);
        lighting.AmbientOut *= textureColor;
        lighting.DiffuseOut *= textureColor;
    }
    else
    {
        lighting.AmbientOut *= material.Ambient;
        lighting.DiffuseOut *= material.Diffuse;
    }

    if (textures.Specular.HasTexture)
    {
        lighting.SpecularOut *= textures.Specular.Texture.Sample(S0_BilinearSampler, textureCoordinates);
    }
    else
    {
        lighting.SpecularOut *= material.Specular;
    }

    return lighting;
}

#endif