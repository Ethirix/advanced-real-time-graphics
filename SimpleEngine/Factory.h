#pragma once
#include <d3d11.h>
#include <memory>
#include <optional>
#include <string>
#include <wrl/client.h>

#include "CreatedMeshes.h"
#include "MeshComponent.h"
#include "MeshType.h"

#define PATH_STR const std::string&
#define DEVICE const Microsoft::WRL::ComPtr<ID3D11Device>&
#define BUFFER Microsoft::WRL::ComPtr<ID3D11Buffer>
#define SHARED_PTR_MESH std::shared_ptr<Mesh>
#define OPTIONAL_SHARED_PTR_MESH std::optional<SHARED_PTR_MESH>
#define OPTIONAL_SHARED_PTR_MESH std::optional<SHARED_PTR_MESH>
#define OPTIONAL_BUFFER std::optional<BUFFER>

class Factory
{
#pragma region Mesh Functions
public:
	static OPTIONAL_SHARED_PTR_MESH CreateMesh(PATH_STR meshPath, DEVICE device, MeshType type);
private:
	static OPTIONAL_SHARED_PTR_MESH DoesMeshExist(PATH_STR path);

	static OPTIONAL_SHARED_PTR_MESH WavefrontOBJLoader(PATH_STR path, DEVICE device, MeshType type);

	static OPTIONAL_BUFFER InitializeVertexBuffer(PATH_STR path, const SHARED_PTR_MESH& mesh, DEVICE device);

	static OPTIONAL_BUFFER InitializeIndexBuffer(PATH_STR path, const SHARED_PTR_MESH& mesh, DEVICE device);

	inline static CreatedMeshes _createdMeshes {};
#pragma endregion

#pragma region Material Functions
public:
	static Material CreateMaterial(PATH_STR materialPath);
private:

#pragma endregion

#pragma region Shader Functions
public:
	static 
#pragma endregion
};
