#ifndef __TEXTURES_HLSLI__
#define __TEXTURES_HLSLI__

#include "Structs/Texture.hlsli"

struct Textures
{
    Texture Diffuse;
    Texture Specular;
    Texture Normal;
};

Textures CreateTextures(Texture diffuse, Texture specular, Texture normal)
{
    Textures textures;
    textures.Diffuse = diffuse;
    textures.Specular = specular;
    textures.Normal = normal;

    return textures;
}

Textures CreateTexturesFromTextures(Texture2D diffuseTexture, bool hasDiffuseTexture, 
	Texture2D specularTexture, bool hasSpecularTexture, Texture2D normalTexture, bool hasNormalTexture)
{
    Texture diffuse = CreateTexture(diffuseTexture, hasDiffuseTexture);
    Texture specular = CreateTexture(specularTexture, hasSpecularTexture);
    Texture normal = CreateTexture(normalTexture, hasNormalTexture);

    return CreateTextures(diffuse, specular, normal);
}

#endif