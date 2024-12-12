#ifndef __TEXTURES_HLSLI__
#define __TEXTURES_HLSLI__

#include "Structs/Texture.hlsli"

struct Textures
{
    Texture Diffuse;
    Texture Specular;
};

Textures CreateTextures(Texture diffuse, Texture specular)
{
    Textures textures;
    textures.Diffuse = diffuse;
    textures.Specular = specular;

    return textures;
}

Textures CreateTexturesFromTextures(Texture2D diffuseTexture, bool hasDiffuseTexture, 
	Texture2D specularTexture, bool hasSpecularTexture)
{
    Texture diffuse = CreateTexture(diffuseTexture, hasDiffuseTexture);
    Texture specular = CreateTexture(specularTexture, hasSpecularTexture);

    return CreateTextures(diffuse, specular);
}

#endif