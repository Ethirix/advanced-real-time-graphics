#pragma once
#include <DirectXMath.h>
#include <memory>
#include <optional>
#include <list>
#include <string>

#define SP_MATERIAL std::shared_ptr<Material>
#define OPT_SP_MATERIAL std::optional<SP_MATERIAL>
#define LOADED_MATERIALS std::list<std::pair<std::string, SP_MATERIAL>>
#define PATH_STR const std::string&

class Material
{
public:
	std::string Name;
	DirectX::XMFLOAT4 Ambient; //Ka
	DirectX::XMFLOAT4 Diffuse; //Kd
	DirectX::XMFLOAT4 Specular; //Kd
	float SpecularExponent; //Ns
	float Transparency; //d

	static OPT_SP_MATERIAL MaterialFromPath(PATH_STR path);

	static void ClearUnusedMaterials();

private:
	inline static LOADED_MATERIALS _loadedMaterials{};

	static OPT_SP_MATERIAL IsMaterialLoaded(PATH_STR path);

	static OPT_SP_MATERIAL LoadMaterialFromPath(PATH_STR path);
};
