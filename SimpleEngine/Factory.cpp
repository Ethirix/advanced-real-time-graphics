#include "Factory.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

#include "DataStore.h"

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
	std::vector<UINT> positionIndices, normalIndices, textCoordIndices;
	std::vector<DirectX::XMFLOAT2> textureCoordinates;

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
			DirectX::XMFLOAT3 v3;
			stringStream >> v3.x;
			stringStream >> v3.y;
			stringStream >> v3.z;
			positions.emplace_back(v3);
		}
		else if (objToken == "vn")
		{
			DirectX::XMFLOAT3 v3;
			stringStream >> v3.x;
			stringStream >> v3.y;
			stringStream >> v3.z;
			normals.emplace_back(v3);
		}
		else if (objToken == "vt")
		{
			DirectX::XMFLOAT2 v2;
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
			std::vector<UINT> data;
			USHORT polygonSideCount = 0;

			while (true)
			{
				std::string indicesLine;
				stringStream >> indicesLine;

				if (indicesLine.empty())
					break;

				polygonSideCount++;
				if (polygonSideCount > 3)
					return {};

				std::ranges::replace(indicesLine, '/', ' ');
				std::istringstream indicesStringStream(indicesLine);

				UINT index;
				while (indicesStringStream >> index)
					data.emplace_back(index - 1);

				positionIndices.emplace_back(data[0]);
				textCoordIndices.emplace_back(data[1]);
				normalIndices.emplace_back(data[2]);
				data.clear();
			}
		}
	}

	mesh->Vertices.Elements = new class Vertex[positionIndices.size()];
	mesh->Vertices.Length = positionIndices.size();
	for (int i = 0; i < positionIndices.size(); i++)
	{
		mesh->Vertices.Elements[i].Position = positions[positionIndices[i]];
		mesh->Vertices.Elements[i].Normal = normals[normalIndices[i]];
		mesh->Vertices.Elements[i].TextureCoordinate = textureCoordinates[textCoordIndices[i]];
	}

	mesh->VertexIndices.Elements = new UINT[mesh->Vertices.Length];
	mesh->VertexIndices.Length = mesh->Vertices.Length;
	for (int i = 0; i < mesh->Vertices.Length; i++)
		mesh->VertexIndices.Elements[i] = i;



	OPTIONAL_BUFFER vertexBuffer = InitializeVertexBuffer(path, mesh, device);
	if (!vertexBuffer.has_value())
		return {};
	mesh->VertexBuffer = vertexBuffer.value();

	OPTIONAL_BUFFER indexBuffer = InitializeIndexBuffer(path, mesh, device);
	if (!indexBuffer.has_value())
		return {};
	mesh->IndexBuffer = indexBuffer.value();

	return mesh;
}

OPTIONAL_BUFFER Factory::InitializeVertexBuffer(PATH_STR path, const SHARED_PTR_MESH& mesh, DEVICE device)
{
	BUFFER buffer = BUFFER();

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.ByteWidth = sizeof(class Vertex) * mesh->Vertices.Length;
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