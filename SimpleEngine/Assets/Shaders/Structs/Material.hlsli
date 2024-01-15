#ifndef __MATERIAL_HLSLI__
#define __MATERIAL_HLSLI__

struct Material
{
    float4 Diffuse;
    float4 Ambient;
    float4 Specular;
    float SpecularExponent;
};

Material CreateMaterial(float4 diffuse, float4 ambient, float4 specular, float specularExponent)
{
    Material material;
    material.Diffuse = diffuse;
    material.Ambient = ambient;
    material.Specular = specular;
    material.SpecularExponent = specularExponent;

    return material;
}

#endif