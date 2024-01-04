Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);
SamplerState bilinearSampler : register(s0);

struct LightData
{
    float4 Position;
    float3 DiffuseColor;
    float DiffusePower;
    float3 SpecularColor;
    float SpecularPower;
    float3 AmbientColor;
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;
    float LightRadius;
	float ___Pad2;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 Projection;
    float4x4 View;
    float4x4 World;
    float4 AmbientMaterial;
    float4 DiffuseMaterial;
    uint HasDiffuseTexture;
    uint HasSpecularTexture;
    float2 ___Pad0;
    float4 SpecularMaterial;
    float SpecularExponent;
    float3 CameraPosition;
}

cbuffer LightBuffer : register(b1)
{
    uint ActiveLightCount;
    float3 ___Pad1;
    LightData PointLights[32];
}

struct LightingOut
{
    float3 AmbientOut;
    float3 DiffuseOut;
    float3 SpecularOut;
};

struct VS_Out
{
    float4 Color : COLOR;

    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION0;

    float4 Normal : NORMAL0;
    float4 WorldNormal : WORLDNORMAL0;

    float2 TextureCoordinates : TEXCOORDS0;
};

LightingOut CalculatePointLight(
	LightData light,
	float3 fragmentPosition,
	float3 cameraPosition,
	float3 cameraDirection,
	float3 normal,
	float2 textureCoordinates)
{
    LightingOut lighting;
    lighting.DiffuseOut = float3(0, 0, 0);
    lighting.SpecularOut = float3(0, 0, 0);
    lighting.AmbientOut = float3(0, 0, 0);

    if (light.DiffusePower <= 0)
        return lighting;

    float3 lightDir = normalize(light.Position - fragmentPosition);
    float lightDirDistance = length(lightDir);
    float lightDirDistSquared = lightDirDistance * lightDirDistance;

    float distance = length(light.Position - fragmentPosition);

    if (light.LightRadius <= 0 && light.LinearAttenuation <= 0 && light.QuadraticAttenuation <= 00)
    {
        float3 rayDirection = normalize(reflect(normalize(light.Position), normal));

        lighting.AmbientOut = light.AmbientColor;
        lighting.DiffuseOut = light.DiffuseColor * light.DiffusePower
    		* saturate(dot(normal, rayDirection));

        if (length(lighting.DiffuseOut > 0) && SpecularExponent > 0)
        {
            float3 halfVector = normalize(rayDirection + cameraDirection);

            lighting.SpecularOut = light.SpecularColor * light.SpecularPower
        		* pow(max(dot(halfVector, cameraDirection), 0.0f), SpecularExponent);
        }
    }
    else
    {
        float attenuation = 1 /
			(light.ConstantAttenuation + abs(light.LinearAttenuation * distance) +
        pow(abs(light.QuadraticAttenuation), 2) * pow(distance, 2));

        if (light.LightRadius > 0)
        {
            attenuation *= clamp(1.0 - pow(distance, 2) / pow(light.LightRadius, 2.0), 0.0, 1.0);
        }

        float diffuseIntensity = saturate(dot(normal, lightDir));
        lighting.DiffuseOut = diffuseIntensity * light.DiffuseColor * light.DiffusePower / lightDirDistSquared;

        if (length(lighting.DiffuseOut > 0) && SpecularExponent > 0)
        {
            float3 halfVector = normalize(lightDir + cameraDirection);
            float specularIntensity = pow(saturate(dot(normal, halfVector)), SpecularExponent);
            lighting.SpecularOut = specularIntensity * light.SpecularColor * light.SpecularPower / lightDirDistSquared;
        }
	
        lighting.AmbientOut = light.AmbientColor;

        lighting.DiffuseOut = lighting.DiffuseOut * attenuation;
        lighting.SpecularOut = lighting.SpecularOut * attenuation;
        lighting.AmbientOut = lighting.AmbientOut * attenuation;
  
    }

    if (HasDiffuseTexture == 1)
    {
        float4 textureColor = diffuseTexture.Sample(bilinearSampler, textureCoordinates);
        lighting.AmbientOut *= textureColor;
        lighting.DiffuseOut *= textureColor;
    }
    else
    {
        lighting.AmbientOut *= AmbientMaterial;
        lighting.DiffuseOut *= DiffuseMaterial;
    }

    if (HasSpecularTexture == 1)
    {
        lighting.SpecularOut *= specularTexture.Sample(bilinearSampler, textureCoordinates);
    }
    else
    {
        lighting.SpecularOut *= SpecularMaterial;
    }

	return lighting;
}

VS_Out VS_main(
	float3 Position : POSITION,
	float3 NormalVector : NORMAL,
	float2 TexCoords : TEXCOORDS)
{
    VS_Out output = (VS_Out)0;

    float4 Pos4 = float4(Position, 1.0f);
    output.Position = mul(Pos4, World);
    output.WorldPosition = output.Position;
    output.WorldNormal = normalize(mul(float4(NormalVector, 0) , World));
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.Normal = (NormalVector, 0);
    output.TextureCoordinates = TexCoords;

    return output;
}

float4 PS_main(VS_Out Input) : SV_TARGET
{
    Input.WorldNormal = normalize(Input.WorldNormal);
    Input.Normal = normalize(Input.Normal);

    float3 cameraDirection = normalize(CameraPosition - Input.WorldPosition);

    float4 ambient = float4(0, 0, 0, 0);
    float4 diffuse = float4(0, 0, 0, 0);
    float4 specular = float4(0, 0, 0, 0);

    for (uint i = 0; i < ActiveLightCount; i++)
    {
        LightingOut light = CalculatePointLight(PointLights[i], Input.WorldPosition, 
			CameraPosition, cameraDirection, Input.WorldNormal, Input.TextureCoordinates);

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