#ifndef __TEXTURE_HLSLI__
#define __TEXTURE_HLSLI__

struct Texture
{
    Texture2D Texture;
    bool HasTexture;
};

Texture CreateTexture(Texture2D text, bool hasTexture)
{
    Texture textureData;
    textureData.Texture = text;
    textureData.HasTexture = hasTexture;

    return textureData;
}

#endif