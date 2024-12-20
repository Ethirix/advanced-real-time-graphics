#include "Factory.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

#include "DataStore.h"
#include "DDSTextureLoader.h"
#include "Helpers.h"
#include "VertexIndices.h"

#pragma region Mesh Functions

OPTIONAL_SHARED_PTR_MESH Factory::CreateMesh(PATH_STR path, MeshType type, DEVICE device)
{
	OPTIONAL_SHARED_PTR_MESH mesh = DoesMeshExist(path);
	if (mesh.has_value())
		return mesh;

	mesh = WavefrontOBJLoader(path, device, type);
	if (mesh.has_value())
	{
		_createdMeshes.Meshes.emplace_back(path, mesh.value());
		return mesh;
	}

	return {};
}

OPTIONAL_SHARED_PTR_MESH Factory::DoesMeshExist(PATH_STR path)
{
	for (LOADED_MESHES::iterator i = _createdMeshes.Meshes.begin();
			i != _createdMeshes.Meshes.end(); ++i)
	{
		if (i->first == path)
			return i->second;
	}

	return {};
}

OPTIONAL_SHARED_PTR_MESH Factory::WavefrontOBJLoader(PATH_STR path, DEVICE device, MeshType type)
{
	SP_MESH mesh = std::make_shared<Mesh>();

	std::ifstream fileStream;
	std::string currentLine;

	fileStream.open(path);
	if (!fileStream.is_open())
		return {};

	std::vector<DirectX::XMFLOAT3> positions, normals;
	std::vector<DirectX::XMFLOAT2> textureCoordinates;

	std::unordered_map<std::string, VertexIndices> faces{};
	std::vector<UINT> indices;
	UINT currentIndex = 0;

	while (std::getline(fileStream, currentLine))
	{
		std::istringstream stringStream(currentLine);
		std::string objToken;
		stringStream >> objToken;

		if (objToken == "o")
		{
			stringStream >> mesh->Name;
		}
		else if (objToken == "v")
		{
			DirectX::XMFLOAT3 v3{};
			stringStream >> v3.x;
			stringStream >> v3.y;
			stringStream >> v3.z;
			positions.emplace_back(v3);
		}
		else if (objToken == "vn")
		{
			DirectX::XMFLOAT3 v3{};
			stringStream >> v3.x;
			stringStream >> v3.y;
			stringStream >> v3.z;
			normals.emplace_back(v3);
		}
		else if (objToken == "vt")
		{
			DirectX::XMFLOAT2 v2{};
			stringStream >> v2.x;
			stringStream >> v2.y;

			if (type == RightHand)
				v2.y = 1.0f - v2.y;

			textureCoordinates.emplace_back(v2);
		}
		else if (objToken == "s")
		{
			stringStream >> mesh->SmoothShaded;
		}
		else if (objToken == "f")
		{
			VertexIndices currentFace{};
			USHORT polygonSideCount = 0;

			while (true)
			{
				std::string indicesLine;
				stringStream >> indicesLine;

				if (indicesLine.empty())
					break;

				if (polygonSideCount++ > 3)
					return {};

				if (faces.contains(indicesLine))
				{
					indices.emplace_back(faces.find(indicesLine)->second.Index);
					continue;
				}

				std::string replacedLine = indicesLine;
				std::ranges::replace(replacedLine, '/', ' ');
				std::istringstream indicesStringStream(replacedLine);

				UINT index;
				indicesStringStream >> index;
				currentFace.Position = index - 1;
				indicesStringStream >> index;
				currentFace.TextureCoordinate = index - 1;
				indicesStringStream >> index;
				currentFace.Normal = index - 1;
				currentFace.Index = currentIndex++;

				faces.try_emplace(indicesLine, currentFace);
				indices.emplace_back(currentFace.Index);
			}
		}
	}

	std::vector<VertexIndices> orderedFaces;
	for (int i = 0; i < faces.size(); ++i)
	{
		for (VertexIndices& vertex : faces | std::views::values)
		{
			if (vertex.Index == i)
			{
				orderedFaces.emplace_back(vertex);
				break;
			}
		}
	}

	mesh->Vertices.Elements = new struct Vertex[faces.size()];
	mesh->Vertices.Length = faces.size();
	{
		unsigned i = 0;
		for (const VertexIndices& vertex : orderedFaces)
		{
			mesh->Vertices.Elements[i].Position = positions[vertex.Position];
			mesh->Vertices.Elements[i].Normal = normals[vertex.Normal];
			mesh->Vertices.Elements[i].TextureCoordinate = textureCoordinates[vertex.TextureCoordinate];

			i++;
		}
	}
	mesh->VertexIndices.Elements = new UINT[indices.size()];
	mesh->VertexIndices.Length = indices.size();
	for (unsigned i = 0; i < indices.size(); ++i)
		mesh->VertexIndices.Elements[i] = indices[i];

	CalculateTangents(mesh);

	OPTIONAL_BUFFER vertexBuffer = InitializeVertexBuffer(path, mesh, device);
	if (!vertexBuffer.has_value())
		return {};
	mesh->VertexBuffer = vertexBuffer.value();

	OPTIONAL_BUFFER indexBuffer = InitializeIndexBuffer(path, mesh, device);
	if (!indexBuffer.has_value())
		return {};
	mesh->IndexBuffer = indexBuffer.value();


	Vector3 min, max;
	for (unsigned i = 0; i < mesh->Vertices.Length; i++)
	{
		Vector3 element = mesh->Vertices.Elements[i].Position;
		if (element.X < min.X)
			min.X = element.X;
		if (element.Y < min.Y)
			min.Y = element.Y;
		if (element.Z < min.Z)
			min.Z = element.Z;

		if (element.X > max.X)
			max.X = element.X;
		if (element.Y > max.Y)
			max.Y = element.Y;
		if (element.Z > max.Z)
			max.Z = element.Z;
	}

	mesh->Bounds.Min = min;
	mesh->Bounds.Max = max;
	mesh->Bounds.Extents = max;
	mesh->Bounds.Size = max * 2;
	mesh->Bounds.Center = Vector3::Zero();

	return mesh;
}

void Factory::CalculateTangents(SHARED_PTR_MESH& mesh, bool recalculateNormals)
{
	//TODO: Add Tangent and Bitangent loading
	//TODO: Add own normal calculation to Tangent/Bitangent function
	//https://marti.works/posts/post-calculating-tangents-for-your-mesh/post/
	//https://terathon.com/blog/tangent-space.html

	std::vector<Vector3> tangents(mesh->Vertices.Length);
	std::vector<Vector3> bitangents(mesh->Vertices.Length);

	for (unsigned i = 0; i < mesh->VertexIndices.Length; i += 3)
	{
		//Indices to triangles
		unsigned index0 = mesh->VertexIndices.Elements[i + 0];
		unsigned index1 = mesh->VertexIndices.Elements[i + 1];
		unsigned index2 = mesh->VertexIndices.Elements[i + 2];

		DirectX::XMFLOAT3 vertexPos0 = mesh->Vertices.Elements[index0].Position;
		DirectX::XMFLOAT3 vertexPos1 = mesh->Vertices.Elements[index1].Position;
		DirectX::XMFLOAT3 vertexPos2 = mesh->Vertices.Elements[index2].Position;

		DirectX::XMFLOAT2 textureCoordinate0 = mesh->Vertices.Elements[index0].TextureCoordinate;
		DirectX::XMFLOAT2 textureCoordinate1 = mesh->Vertices.Elements[index1].TextureCoordinate;
		DirectX::XMFLOAT2 textureCoordinate2 = mesh->Vertices.Elements[index2].TextureCoordinate;

		DirectX::XMFLOAT3 edge0 = (Vector3(vertexPos1) - Vector3(vertexPos0)).ToDXFloat3();
		DirectX::XMFLOAT3 edge1 = (Vector3(vertexPos2) - Vector3(vertexPos0)).ToDXFloat3();

		DirectX::XMFLOAT2 uv0 = { textureCoordinate1.x - textureCoordinate0.x, textureCoordinate1.y - textureCoordinate0.y };
		DirectX::XMFLOAT2 uv1 = { textureCoordinate2.x - textureCoordinate0.x, textureCoordinate2.y - textureCoordinate0.y };

		float result = 1.0f / (uv0.x * uv1.y - uv0.y * uv1.x);

		DirectX::XMFLOAT3 tangent
		{
			(edge0.x * uv1.y - edge1.x * uv0.y) * result,
			(edge0.y * uv1.y - edge1.y * uv0.y) * result,
			(edge0.z * uv1.y - edge1.z * uv0.y) * result
		};

		DirectX::XMFLOAT3 biTangent
		{
			(edge0.x * uv1.x - edge1.x * uv0.x) * result,
			(edge0.y * uv1.x - edge1.y * uv0.x) * result,
			(edge0.z * uv1.x - edge1.z * uv0.x) * result
		};

		//Additive to get an 'average' as vertices are deduplicated
		tangents[index0] += tangent;
		tangents[index1] += tangent;
		tangents[index2] += tangent;

		bitangents[index0] += biTangent;
		bitangents[index1] += biTangent;
		bitangents[index2] += biTangent;
	}

	for (unsigned i = 0; i < mesh->Vertices.Length; i++)
	{
		Vector3 normal = mesh->Vertices.Elements[i].Normal;
		Vector3 tangent = tangents[i];
		Vector3 bitangent = bitangents[i];

		Vector3 normalisedTangent = (tangent - normal * normal.Dot(tangent)).Normalise();
		Vector3 normalisedBitangent = (bitangent - normal * normal.Dot(bitangent)).Normalise();

		float handedness = Vector3::Cross(normal, tangent).Dot(bitangent) > 0 ? 1.0f : -1.0f;

		mesh->Vertices.Elements[i].Tangent = { normalisedTangent.X, normalisedTangent.Y, normalisedTangent.Z, handedness };
		mesh->Vertices.Elements[i].Bitangent = { normalisedBitangent.X, normalisedBitangent.Y, normalisedBitangent.Z, handedness };
	}
}

OPTIONAL_BUFFER Factory::InitializeVertexBuffer(PATH_STR path, const SHARED_PTR_MESH& mesh, DEVICE device)
{
	BUFFER buffer = BUFFER();

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.ByteWidth = sizeof(struct Vertex) * mesh->Vertices.Length;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceVertexData = { mesh->Vertices.Elements };

	HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &subresourceVertexData, &buffer);
	if (FAILED(hr))
		return {};

	_createdMeshes.VertexBuffers.emplace_back(path, buffer);

	return buffer;
}

OPTIONAL_BUFFER Factory::InitializeIndexBuffer(PATH_STR path, const SHARED_PTR_MESH& mesh, DEVICE device)
{
	BUFFER buffer = BUFFER();

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.ByteWidth = sizeof(*mesh->VertexIndices.Elements) * mesh->VertexIndices.Length; //size of the index data
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceIndexData = { mesh->VertexIndices.Elements };

	HRESULT hr = device->CreateBuffer(&indexBufferDesc, &subresourceIndexData, &buffer);
	if (FAILED(hr))
		return {};

	_createdMeshes.IndexBuffers.emplace_back(path, buffer);

	return buffer;
}

#pragma endregion

#pragma region Material Functions

OPTIONAL_SHARED_PTR_MTL Factory::CreateMaterial(PATH_STR path)
{
	if (OPTIONAL_SHARED_PTR_MTL mtl = WavefrontMTLLoader(path); mtl.has_value())
		return mtl;
	return {};
}

OPTIONAL_SHARED_PTR_MTL Factory::WavefrontMTLLoader(PATH_STR path)
{
	SHARED_PTR_MTL material = std::make_shared<Material>();

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
			DirectX::XMFLOAT4 v4{};
			stringStream >> v4.x;
			stringStream >> v4.y;
			stringStream >> v4.z;
			v4.w = 1;

			material->Ambient = v4;
		}
		else if (objToken == "Kd")
		{
			DirectX::XMFLOAT4 v4{};
			stringStream >> v4.x;
			stringStream >> v4.y;
			stringStream >> v4.z;
			v4.w = 1;

			material->Diffuse = v4;
		}
		else if (objToken == "Ks")
		{
			DirectX::XMFLOAT4 v4{};
			stringStream >> v4.x;
			stringStream >> v4.y;
			stringStream >> v4.z;
			v4.w = 1;

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

#pragma endregion

#pragma region Shader Functions

bool Factory::LoadVertexShader(PATH_STR path, SHARED_PTR_MTL material)
{
	OPTIONAL_SHADER_DATA_VERTEX vertex = DataStore::VertexShaders.Retrieve(path);
	if (vertex.has_value())
		material->Shader.VertexShader = vertex.value();

	return vertex.has_value();
}

bool Factory::LoadPixelShader(PATH_STR path, SHARED_PTR_MTL material)
{
	OPTIONAL_SHADER_DATA_PIXEL pixel = DataStore::PixelShaders.Retrieve(path);
	if (pixel.has_value())
		material->Shader.PixelShader = pixel.value();

	return pixel.has_value();
}

#pragma endregion

#pragma region Texture Functions

std::optional<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Factory::CreateTexture(PATH_STR path, DEVICE device)
{
	if (auto resource = DoesTextureExist(path); resource.has_value())
		return resource;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resource = {};
	auto wPath = Helpers::StringToWideString(path);

	if (std::filesystem::path fsPath { wPath }; exists(fsPath))
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(device.Get(), wPath, 
				nullptr, resource.GetAddressOf());

		if (FAILED(hr))
			return {};
	}
	else
		return {};

	DataStore::Resources.Store(path, resource);
	return resource;
}

std::optional<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Factory::DoesTextureExist(const std::string& path)
{
	if (auto resource = DataStore::Resources.Retrieve(path); resource.has_value())
		return resource;

	return {};
}


#pragma endregion