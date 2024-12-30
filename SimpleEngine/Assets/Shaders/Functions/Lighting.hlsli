#ifndef __LIGHTING_HLSLI__
#define __LIGHTING_HLSLI__

#include "Buffers/CB0_ObjectCameraData.hlsli"
#include "Buffers/CB3_Lighting.hlsli"

#include "Samplers/S0_BilinearSampler.hlsli"
#include "Structs/LightingOut.hlsli"
#include "Structs/Material.hlsli"
#include "Structs/Textures.hlsli"

#include "Structured Resources/T8_DirectionalLights.hlsli"
#include "Structured Resources/T9_PointLights.hlsli"
#include "Structured Resources/T10_SpotLights.hlsli"

LightingOut CalculateTextures(LightingOut lighting, float2 textureCoordinates, Textures textures, Material material)
{
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

LightingOut CalculateDirectionalLight(
	DirectionalLightData light, 
	float3 fragmentPosition,
	float3 normal,
	Material material)
{
	LightingOut lighting;
    lighting.DiffuseOut = float3(0, 0, 0);
    lighting.SpecularOut = float3(0, 0, 0);
    lighting.AmbientOut = light.AmbientColor;

    if (light.DiffusePower <= 0.0f)
        return lighting;

    float3 rayDirection = normalize(reflect(normalize(light.Direction), normal));
    lighting.DiffuseOut = light.DiffuseColor * light.DiffusePower
    		* saturate(dot(normal, rayDirection));

    if (light.SpecularPower <= 0.0f)
        return lighting;

    float3 eyeDirection = normalize(Eye.xyz - fragmentPosition);

	float3 halfVector = normalize(rayDirection + eyeDirection);
    lighting.SpecularOut = light.SpecularColor * light.SpecularPower
		* pow(saturate(dot(halfVector, eyeDirection)), material.SpecularExponent);

    return lighting;
}

LightingOut CalculatePointLight(
	PointLightData light,
	float3 fragmentPosition,
	float3 normal,
	Material material)
{
    LightingOut lighting;
    lighting.DiffuseOut = float3(0, 0, 0);
    lighting.SpecularOut = float3(0, 0, 0);
    lighting.AmbientOut = light.AmbientColor;

    float3 lightDir = normalize(light.Position - fragmentPosition);
    float distance = length(light.Position - fragmentPosition);

    float attenuation = 1 /
			(light.ConstantAttenuation + abs(light.LinearAttenuation * distance) +
        pow(abs(light.QuadraticAttenuation), 2) * pow(distance, 2));

    if (light.LightRadius > 0)
    {
        attenuation *= clamp(1.0 - pow(distance, 2) / pow(light.LightRadius, 2.0), 0.0, 1.0);
    }

    lighting.AmbientOut *= attenuation;

    if (light.DiffusePower <= 0.0f)
        return lighting;

    float diffuseIntensity = saturate(dot(normal, lightDir));
    lighting.DiffuseOut = diffuseIntensity * light.DiffuseColor * light.DiffusePower / length(lightDir);
    lighting.DiffuseOut *= attenuation;

    if (light.SpecularPower <= 0.0f)
        return lighting;

    float3 eyeDirection = normalize(Eye.xyz - fragmentPosition);

    float3 halfVector = normalize(lightDir + eyeDirection);
    float specularIntensity = pow(max(dot(normal, halfVector), 0.0f), material.SpecularExponent);
    lighting.SpecularOut = specularIntensity * light.SpecularColor * light.SpecularPower / length(lightDir);
    lighting.SpecularOut *= attenuation;

    return lighting;
}

LightingOut CalculateSpotLight(
	SpotLightData light,
	float3 fragmentPosition,
	float3 normal,
	Material material)
{
	//TODO: Implement Spotlights
    return (LightingOut) 0;
}

LightingOut CalculateLighting(
	float3 fragmentPosition,
	float2 textureCoordinates,
	float3 worldNormal,
	Textures textures,
	Material material)
{
    LightingOut lighting;
    lighting.DiffuseOut = float3(0, 0, 0);
    lighting.SpecularOut = float3(0, 0, 0);
    lighting.AmbientOut = float3(0, 0, 0);

    for (uint i = 0; i < TotalDirectionalLights && i < MaxDirectionalLights; i++)
    {
        LightingOut light = CalculateDirectionalLight(T8_DirectionalLights[i], fragmentPosition, worldNormal, material);

        lighting.DiffuseOut += light.DiffuseOut;
        lighting.SpecularOut += light.SpecularOut;
        lighting.AmbientOut += light.AmbientOut;
    }

    for (uint i = 0; i < TotalPointLights && i < MaxPointLights; i++)
    {
        LightingOut light = CalculatePointLight(T9_PointLights[i], fragmentPosition, worldNormal, material);

        lighting.DiffuseOut += light.DiffuseOut;
        lighting.SpecularOut += light.SpecularOut;
        lighting.AmbientOut += light.AmbientOut;
    }

    for (uint i = 0; i < TotalSpotLights && i < MaxSpotLights; i++)
    {
        LightingOut light = CalculateSpotLight(T10_SpotLights[i], fragmentPosition, worldNormal, material);
        

        lighting.DiffuseOut += light.DiffuseOut;
        lighting.SpecularOut += light.SpecularOut;
        lighting.AmbientOut += light.AmbientOut;
    }

    lighting = CalculateTextures(lighting, textureCoordinates, textures, material);

    return lighting;
}

#endif