#include "Material.h"

#include <fstream>
#include <sstream>

OPT_SP_MATERIAL Material::MaterialFromPath(PATH_STR path)
{
	OPT_SP_MATERIAL material = IsMaterialLoaded(path);
	if (material.has_value())
		return material;

	material = LoadMaterialFromPath(path);
	if (material.has_value())
	{
		_loadedMaterials.emplace_back(path, material.value());
		return material;
	}
		

	return {};
}

OPT_SP_MATERIAL Material::IsMaterialLoaded(PATH_STR path)
{
	for (LOADED_MATERIALS::iterator i = _loadedMaterials.begin();
		i != _loadedMaterials.end(); ++i)
	{
		if (i->first == path)
			return i->second;
	}

	return {};
}

void Material::ClearUnusedMaterials()
{
	bool removedMaterial = false;

	do
	{
		removedMaterial = false;
		for (auto material : _loadedMaterials)
		{
			if (material.second.use_count() == 2)
			{
				_loadedMaterials.remove_if([material, &removedMaterial](const std::pair<std::string, SP_MATERIAL>& m)
					{
						if (m.first == material.first)
							removedMaterial = true;

						return m.first == material.first;
					});

				if (removedMaterial)
					break;
			}
		}
	}
	while (removedMaterial);
}

OPT_SP_MATERIAL Material::LoadMaterialFromPath(PATH_STR path)
{
	SP_MATERIAL material = std::make_shared<Material>();

	std::ifstream fileStream;
	std::string currentLine;

	fileStream.open(path);
	if (!fileStream.is_open())
		return {};

	while (std::getline(fileStream, currentLine))
	{
		std::istringstream stringStream(currentLine);
		std::string objToken;
		stringStream >> objToken;

		if (objToken == "Ka")
		{
			DirectX::XMFLOAT4 v4;
			stringStream >> v4.x;
			stringStream >> v4.y;
			stringStream >> v4.z;

			material->Ambient = v4;
		}
		else if (objToken == "Kd")
		{
			DirectX::XMFLOAT4 v4;
			stringStream >> v4.x;
			stringStream >> v4.y;
			stringStream >> v4.z;

			material->Diffuse = v4;
		}
		else if (objToken == "Ks")
		{
			DirectX::XMFLOAT4 v4;
			stringStream >> v4.x;
			stringStream >> v4.y;
			stringStream >> v4.z;

			material->Specular = v4;
		}
		else if (objToken == "Ns")
		{
			stringStream >> material->SpecularExponent;
		}
		else if (objToken == "newmtl")
		{
			stringStream >> material->Name;
		}
		else if (objToken == "d")
		{
			stringStream >> material->Transparency;
		}
	}

	return material;
}