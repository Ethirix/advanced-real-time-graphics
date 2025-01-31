#ifndef __LIGHTING_HLSLI__
#define __LIGHTING_HLSLI__

#include "Functions/VectorSpaceConverter.hlsli"

#include "Buffers/CB0_ObjectCameraData.hlsli"
#include "Buffers/CB3_Lighting.hlsli"

#include "Samplers/S0_BilinearSampler.hlsli"
#include "Structs/LightingOut.hlsli"
#include "Structs/Material.hlsli"
#include "Structs/Textures.hlsli"

#include "Structured Resources/T10_SpotLights.hlsli"
#include "Structured Resources/T8_DirectionalLights.hlsli"
#include "Structured Resources/T9_PointLights.hlsli"

float CalculateAttenuation(float distance, float constantAttenuation, float linearAttenuation,
                           float quadraticAttenuation, float radius)
{
    float att = 1 / (constantAttenuation + abs(linearAttenuation * distance) + (abs(quadraticAttenuation) * distance * distance));
    if (radius > 0)
        att *= saturate(1.0 - (distance * distance) / (radius * radius));

    return att;
}

float CalculateSpotLightIntensity(float3 lD, float innerAngle, float outerAngle)
{
    float cosInner = cos(radians(innerAngle));
    float cosOuter = cos(radians(outerAngle));

    float theta = dot(normalize(lD), -lD);
    float epsilon = cosInner - cosOuter;
    return saturate((theta - cosOuter) / epsilon);
}

float3 CalculateNormal(float3x3 tbnMatrix, float2 uv, Texture normalTexture)
{
    if (!normalTexture.HasTexture)
        return normalize(tbnMatrix._m02_m12_m22);

    float4 normalMap = normalTexture.Texture.Sample(S0_BilinearSampler, uv);
    return normalize((normalMap.xyz * 2.0f) - 1.0f);
}

float3 CalculateEye(float3x3 tbnMatrix, float3 eye, float3 fragmentPosition, Texture normalTexture)
{
    if (!normalTexture.HasTexture)
        return eye;

    return ConvertVectorToSpace(eye - fragmentPosition, tbnMatrix);
}

float3 CalculateLightDirection(float3x3 tbnMatrix, float3 direction, Texture normalTexture)
{
    if (!normalTexture.HasTexture)
        return normalize(direction);

    return normalize(ConvertVectorToSpace(direction, tbnMatrix));
}

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

LightingOut CalculateBlinnPhong(
	float3 fragmentPosition, 
	float3 normal,
	float3 eye,
	float3 lightDirection, 
	float distance,
	float3 ambient, 
	float4 diffuse, 
	float4 specular, 
	float specularExponent)
{
    LightingOut lighting = (LightingOut) 0;

    lighting.AmbientOut = ambient;

    float diffuseIntensity = saturate(dot(normal, lightDirection));
    lighting.DiffuseOut = diffuseIntensity * diffuse.rgb * diffuse.a / distance;

    if (diffuseIntensity == 0)
        return lighting;

    float3 eyeDirection = normalize(eye - fragmentPosition);
    float3 halfVector = normalize(lightDirection + eyeDirection);
    float specularIntensity = pow(saturate(dot(normal, halfVector)), specularExponent);
    lighting.SpecularOut = specularIntensity * specular.rgb * specular.a / distance;

    return lighting;
}

LightingOut CalculateDirectionalLight(
	DirectionalLightData light, 
	float3 fragmentPosition,
	float3x3 tbnMatrix,
	float2 uv,
	float3 eye,
	Texture normalTexture,
	Material material)
{
    float3 lightDirection = CalculateLightDirection(tbnMatrix, -light.Direction, normalTexture);
    float3 normal = CalculateNormal(tbnMatrix, uv, normalTexture);

    return CalculateBlinnPhong(fragmentPosition, normal, 
							   //CalculateEye(tbnMatrix, eye, float3(0, 0, 0), normalTexture), 
							   CalculateEye(tbnMatrix, eye, fragmentPosition, normalTexture), 
							   //eye,
							   lightDirection, 1, light.AmbientColor,
                               float4(light.DiffuseColor, light.DiffusePower),
                               float4(light.SpecularColor, light.SpecularPower), material.SpecularExponent);
}

LightingOut CalculatePointLight(
	PointLightData light,
	float3 fragmentPosition,
	float3x3 tbnMatrix,
	float2 uv,
	float3 eye,
	Texture normalTexture,
	Material material)
{
    float3 lightDirection = CalculateLightDirection(tbnMatrix, light.Position - fragmentPosition, normalTexture);
    float3 normal = CalculateNormal(tbnMatrix, uv, normalTexture);
    float distance = length(light.Position - fragmentPosition);

    LightingOut lighting = CalculateBlinnPhong(fragmentPosition, normal, 
							   CalculateEye(tbnMatrix, eye, fragmentPosition, normalTexture), lightDirection, distance, light.AmbientColor,
                               float4(light.DiffuseColor, light.DiffusePower),
                               float4(light.SpecularColor, light.SpecularPower), material.SpecularExponent);

    float attenuation = CalculateAttenuation(distance, light.ConstantAttenuation, light.LinearAttenuation,
                                             light.QuadraticAttenuation, light.LightRadius);

	lighting.AmbientOut *= attenuation;
    lighting.DiffuseOut *= attenuation;
    lighting.SpecularOut *= attenuation;

    return lighting;
}

LightingOut CalculateSpotLight(
	SpotLightData light,
	float3 fragmentPosition,
	float3x3 tbnMatrix,
	float2 uv,
	float3 eye,
	Texture normalTexture,
	Material material)
{

    float3 lightDirection = CalculateLightDirection(tbnMatrix, light.Position - fragmentPosition, normalTexture);
    float3 normal = CalculateNormal(tbnMatrix, uv, normalTexture);
    float distance = length(light.Position - fragmentPosition);

    LightingOut lighting = CalculateBlinnPhong(fragmentPosition, normal,
							   CalculateEye(tbnMatrix, eye, fragmentPosition, normalTexture), lightDirection, distance, light.AmbientColor,
                               float4(light.DiffuseColor, light.DiffusePower),
                               float4(light.SpecularColor, light.SpecularPower), material.SpecularExponent);

    float attenuation = CalculateAttenuation(distance, light.ConstantAttenuation, light.LinearAttenuation,
                                             light.QuadraticAttenuation, light.LightRadius);
    float intensity = CalculateSpotLightIntensity(lightDirection, light.InnerAngle, light.OuterAngle);

    lighting.AmbientOut *= attenuation;
    lighting.DiffuseOut *= attenuation * intensity;
    lighting.SpecularOut *= attenuation * intensity;

    return lighting;
}

LightingOut CalculateLighting(
	float3 fragmentPosition,
	float2 textureCoordinates,
	float3x3 tbnMatrix,
	float2 uv,
	Textures textures,
	Material material)
{
    LightingOut lighting;
    lighting.DiffuseOut = float3(0, 0, 0);
    lighting.SpecularOut = float3(0, 0, 0);
    lighting.AmbientOut = float3(0, 0, 0);

    for (uint i = 0; i < TotalDirectionalLights && i < MaxDirectionalLights; i++)
    {
        LightingOut light = CalculateDirectionalLight(T8_DirectionalLights[i], fragmentPosition, tbnMatrix, uv, 
													  Eye.xyz, textures.Normal, material);

        lighting.DiffuseOut += light.DiffuseOut;
        lighting.SpecularOut += light.SpecularOut;
        lighting.AmbientOut += light.AmbientOut;
    }

    for (uint i = 0; i < TotalPointLights && i < MaxPointLights; i++)
    {
        LightingOut light = CalculatePointLight(T9_PointLights[i], fragmentPosition, tbnMatrix, uv,
													  Eye.xyz, textures.Normal, material);

        lighting.DiffuseOut += light.DiffuseOut;
        lighting.SpecularOut += light.SpecularOut;
        lighting.AmbientOut += light.AmbientOut;
    }

    for (uint i = 0; i < TotalSpotLights && i < MaxSpotLights; i++)
    {
        LightingOut light = CalculateSpotLight(T10_SpotLights[i], fragmentPosition, tbnMatrix, uv,
													  Eye.xyz, textures.Normal, material);

        lighting.DiffuseOut += light.DiffuseOut;
        lighting.SpecularOut += light.SpecularOut;
        lighting.AmbientOut += light.AmbientOut;
    }

    lighting = CalculateTextures(lighting, textureCoordinates, textures, material);

    return lighting;
}

#endif