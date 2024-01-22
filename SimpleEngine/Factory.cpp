#include "Factory.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

OPTIONAL_SHARED_PTR_MESH Factory::CreateMesh(PATH_STR path, DEVICE device)
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
		for (i->first == path)
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
		//TODO: update .mtl loading to use direct path to file instead of reading it from the .obj
		//else if (objToken == "mtllib")
		//{
		//	std::string name;
		//	stringStream >> name;

		//	std::string matPath = "Materials/" + name;
		//	std::optional<std::shared_ptr<::Material>> material = Material::MaterialFromPath(matPath);
		//	if (!material.has_value())
		//		mesh->Material = {};

		//	mesh->Material = material.value();
		//}
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
