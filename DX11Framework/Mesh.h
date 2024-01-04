#pragma once

#include <list>
#include <optional>
#include <memory>
#include <string>
#include <algorithm>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <sstream>
#include <vector>

#include <Windows.h>

#include "Material.h"
#include "SafePointerArray.h"

#include <wrl.h>

#include "MeshType.h"
using Microsoft::WRL::ComPtr;

#define SP_MESH std::shared_ptr<Mesh>
#define OPT_SP_MESH std::optional<SP_MESH>
#define LOADED_MESHES std::list<std::pair<std::string, SP_MESH>>
#define COM_BUFFER ComPtr<ID3D11Buffer>
#define OPT_COM_BUFFER std::optional<COM_BUFFER>
#define LOADED_BUFFERS std::list<std::pair<std::string, COM_BUFFER>>
#define PATH_STR const std::string&

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TextureCoordinate;
};

class Mesh
{
public:
	~Mesh();

	std::string Name;

	SafePointerArray<Vertex> Vertices{};
	SafePointerArray<UINT> VertexIndices{};
	std::shared_ptr<Material> Material;
	bool SmoothShaded;

	COM_BUFFER VertexBuffer;
	COM_BUFFER IndexBuffer;
private:
	

#pragma region Static Members
public:
	static OPT_SP_MESH MeshFromOBJ(PATH_STR path, const ComPtr<ID3D11Device>& device, MeshType type);

	static OPT_COM_BUFFER CreateVertexBuffer(PATH_STR path, const SP_MESH& mesh, const ComPtr<ID3D11Device>& device);
	static OPT_COM_BUFFER CreateIndexBuffer(PATH_STR path, const SP_MESH& mesh, const ComPtr<ID3D11Device>& device);

	static void ClearUnusedMeshes();
private:
	inline static LOADED_MESHES _loadedMeshes{};
	inline static LOADED_BUFFERS _loadedVertexBuffers{};
	inline static LOADED_BUFFERS _loadedIndexBuffers{};

	static OPT_COM_BUFFER IsVertexBufferCreated(PATH_STR path);
	static OPT_COM_BUFFER IsIndexBufferCreated(PATH_STR path);


	static OPT_SP_MESH IsMeshLoaded(PATH_STR path);

	static OPT_SP_MESH LoadFromOBJ(PATH_STR path, const ComPtr<ID3D11Device>& device, MeshType type);
#pragma endregion
};

