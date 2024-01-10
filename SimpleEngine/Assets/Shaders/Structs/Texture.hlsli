struct Texture
{
    Texture2D Texture;
    bool HasTexture;
};

Texture CreateTexture(Texture2D texture, bool hasTexture)
{
    Texture texture;
    texture.Texture = texture;
    texture.HasTexture = hasTexture;

    return texture;
}