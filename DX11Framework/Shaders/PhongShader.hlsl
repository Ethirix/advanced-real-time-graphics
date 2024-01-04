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

cbuffer ConstantBuffer : register(b2)
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
    LightData GlobalLight;
}

struct VS_Out
{
    float4 Color : COLOR;

	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION0;

	float4 Normal : NORMAL0;
    float4 WorldNormal : WORLDNORMAL0;

    float2 TextureCoordinates : TEXCOORDS0;
};

VS_Out VS_main(
	float3 Position : POSITION,
	float3 Normal : NORMAL,
	float2 TexCoords : TEXCOORDS)
{   
    VS_Out output = (VS_Out) 0;

    float4 Pos4 = float4(Position, 1.0f);
    output.Position = mul(Pos4, World);
    output.WorldPosition = output.Position;
    output.WorldNormal = normalize(mul(float4(Normal, 0), World));
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.TextureCoordinates = TexCoords;

	output.Color = float4(0, 0, 0, 0);

    return output;
}

float4 PS_main(VS_Out Input) : SV_TARGET
{
    Input.WorldNormal = normalize(Input.WorldNormal);
    Input.Normal = normalize(Input.Normal);

    float3 viewerDirection = normalize(CameraPosition - Input.WorldPosition);
    float3 rayDirection = normalize(reflect(normalize(GlobalLight.Position), Input.WorldNormal));

    float4 ambient = float4(0, 0, 0, 0);
    float4 diffuse = float4(0, 0, 0, 0);
    float4 specular = float4(0, 0, 0, 0);

    if (HasDiffuseTexture == 1)
    {
        float4 texColor = diffuseTexture.Sample(bilinearSampler, Input.TextureCoordinates);

        ambient = texColor * float4(GlobalLight.AmbientColor, 0);
        diffuse = (texColor * float4(GlobalLight.DiffuseColor, 0) * GlobalLight.DiffusePower)
    		* saturate(dot(Input.WorldNormal, -normalize(GlobalLight.Position)));
    }
    else
    {
        ambient = AmbientMaterial * float4(GlobalLight.AmbientColor, 0);
        diffuse = (DiffuseMaterial * float4(GlobalLight.DiffuseColor, 0) * GlobalLight.DiffusePower)
    		* saturate(dot(Input.WorldNormal, -normalize(GlobalLight.Position)));
    }
	
    if (SpecularExponent > 0 && HasSpecularTexture == 0 && length(diffuse) > 0)
    {
        specular = (SpecularMaterial * float4(GlobalLight.SpecularColor * 8, 1) * GlobalLight.SpecularPower)
    		* pow(saturate(dot(rayDirection, viewerDirection)), SpecularExponent / 4); //shinyness
    }
    else if (SpecularExponent > 0 && HasSpecularTexture == 1 && length(diffuse) > 0)
    {
        float4 specColor = specularTexture.Sample(bilinearSampler, Input.TextureCoordinates);

        specular = (specColor * float4(GlobalLight.SpecularColor * 8, 1) * GlobalLight.SpecularPower)
    		* pow(saturate(dot(rayDirection, viewerDirection)), SpecularExponent / 4);
    }

    float4 color = float4(0, 0, 0, 0);
	color += ambient;
    color += diffuse;
    color += specular;
    
    return color;
}