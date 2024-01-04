#include "Textures.h"

#include <filesystem>

#include "DDSTextureLoader.h"
#include "Helpers.h"

OPT_SP_TEXTURES Textures::CreateTextures(PATH_STR path, const ComPtr<ID3D11Device>& device)
{
	if (path == "")
		return {};

	OPT_SP_TEXTURES textures = IsTexturesLoaded(path);
	if (textures.has_value())
		return textures;

	textures = LoadTexturesFromPath(path, device);
	if (textures.has_value())
	{
		_loadedTextures.emplace_back(path, textures.value());
		return textures;
	}


	return {};
}

OPT_SP_TEXTURES Textures::IsTexturesLoaded(PATH_STR path)
{
	for (LOADED_TEXTURES::iterator i = _loadedTextures.begin();
		i != _loadedTextures.end(); ++i)
	{
		if (i->first == path)
			return i->second;
	}

	return {};
}

OPT_SP_TEXTURES Textures::LoadTexturesFromPath(PATH_STR path, const ComPtr<ID3D11Device>& device)
{
	SP_TEXTURES textures = std::make_shared<Textures>(Textures());
	HRESULT hr = S_OK;

	std::string diffusePath = "Textures\\" + path + "\\" + path + "_DIFFUSE.dds";
	std::string displacementPath = "Textures\\" + path + "\\" + path + "_DISPLACEMENT.dds";
	std::string normalPath = "Textures\\" + path + "\\" + path + "_NORMAL.dds";
	std::string specularPath = "Textures\\" + path + "\\" + path + "_SPECULAR.dds";

	CreateTexture(diffusePath, device, textures->Diffuse);
	CreateTexture(displacementPath, device, textures->Displacement);
	CreateTexture(normalPath, device, textures->Normal);
	CreateTexture(specularPath, device, textures->Specular);

	return textures;
}

void Textures::ClearUnusedTextures()
{
	bool removedTextures = false;

	do
	{
		removedTextures = false;
		for (auto textures : _loadedTextures)
		{
			if (textures.second.use_count() == 2)
			{
				_loadedTextures.remove_if([textures, &removedTextures]
				(const std::pair<std::string, SP_TEXTURES>& m)
					{
						if (m.first == textures.first)
							removedTextures = true;

						return m.first == textures.first;
					});

				if (removedTextures)
					break;
			}
		}
	} while (removedTextures);
}

bool Textures::CreateTexture(PATH_STR path, const ComPtr<ID3D11Device>& device, 
	ComPtr<ID3D11ShaderResourceView>& texture)
{
	auto wPath = Helpers::StringToWideString(path);

	std::filesystem::path fsPath { wPath };
	if (exists(fsPath))
	{
		HRESULT hr = 
			DirectX::CreateDDSTextureFromFile(device.Get(), wPath, 
				nullptr, texture.GetAddressOf());

		if (FAILED(hr))
			texture = nullptr;
	}

	return texture != nullptr;
}