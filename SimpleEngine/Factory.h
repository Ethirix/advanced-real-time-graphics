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
#define MTL Material
#define DEVICE const Microsoft::WRL::ComPtr<ID3D11Device>&
#define BUFFER Microsoft::WRL::ComPtr<ID3D11Buffer>
#define SHARED_PTR_MESH std::shared_ptr<Mesh>
#define SHARED_PTR_MTL std::shared_ptr<MTL>
#define SHARED_PTR_RESOURCE Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>
#define OPTIONAL_SHARED_PTR_MESH std::optional<SHARED_PTR_MESH>
#define OPTIONAL_SHARED_PTR_MTL std::optional<SHARED_PTR_MTL>
#define OPTIONAL_SHARED_PTR_RESOURCE std::optional<SHARED_PTR_RESOURCE>
#define OPTIONAL_BUFFER std::optional<BUFFER>

class Factory
{
#pragma region Mesh Functions
public:
	static OPTIONAL_SHARED_PTR_MESH CreateMesh(PATH_STR path, MeshType type, DEVICE device);
private:
	static OPTIONAL_SHARED_PTR_MESH DoesMeshExist(PATH_STR path);

	static OPTIONAL_SHARED_PTR_MESH WavefrontOBJLoader(PATH_STR path, DEVICE device, MeshType type);

	static OPTIONAL_BUFFER InitializeVertexBuffer(PATH_STR path, const SHARED_PTR_MESH& mesh, DEVICE device);

	static OPTIONAL_BUFFER InitializeIndexBuffer(PATH_STR path, const SHARED_PTR_MESH& mesh, DEVICE device);

	inline static CreatedMeshes _createdMeshes {};
#pragma endregion

#pragma region Material Functions
public:
	static OPTIONAL_SHARED_PTR_MTL CreateMaterial(PATH_STR path);
private:
	static OPTIONAL_SHARED_PTR_MTL WavefrontMTLLoader(PATH_STR path);
#pragma endregion

#pragma region Shader Functions
public:
	static bool LoadVertexShader(PATH_STR path, SHARED_PTR_MTL material);

	static bool LoadPixelShader(PATH_STR path, SHARED_PTR_MTL material);
private:
#pragma endregion

#pragma region Texture Functions
public:
	static OPTIONAL_SHARED_PTR_RESOURCE CreateTexture(PATH_STR path, DEVICE device);
private:
	static OPTIONAL_SHARED_PTR_RESOURCE DoesTextureExist(PATH_STR path);
#pragma endregion
};
