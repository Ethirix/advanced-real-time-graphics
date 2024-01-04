#pragma once
#include <wrl.h>
#include <memory>
#include <optional>
#include <list>
#include <string>
#include <d3d11_1.h>

using Microsoft::WRL::ComPtr;

#define SP_TEXTURES std::shared_ptr<Textures>
#define OPT_SP_TEXTURES std::optional<SP_TEXTURES>
#define LOADED_TEXTURES std::list<std::pair<std::string, SP_TEXTURES>>
#define PATH_STR const std::string&

class Textures
{
public:
	std::string Name;
	ComPtr<ID3D11ShaderResourceView> Diffuse;
	ComPtr<ID3D11ShaderResourceView> Displacement;
	ComPtr<ID3D11ShaderResourceView> Normal;
	ComPtr<ID3D11ShaderResourceView> Specular;

	static OPT_SP_TEXTURES CreateTextures(PATH_STR path, const ComPtr<ID3D11Device>& device);

	static void ClearUnusedTextures();
private:
	Textures() = default;

	inline static LOADED_TEXTURES _loadedTextures{};

	static OPT_SP_TEXTURES IsTexturesLoaded(PATH_STR path);

	static OPT_SP_TEXTURES LoadTexturesFromPath(PATH_STR path,
		const ComPtr<ID3D11Device>& device);

	static bool CreateTexture(PATH_STR path, const ComPtr<ID3D11Device>& device, 
		ComPtr<ID3D11ShaderResourceView>& texture);
};

