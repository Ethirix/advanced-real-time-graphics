#include "Mesh.h"

Mesh::~Mesh() = default;

OPT_SP_MESH Mesh::MeshFromOBJ(PATH_STR path, const ComPtr<ID3D11Device>& device, MeshType type)
{
	OPT_SP_MESH mesh = IsMeshLoaded(path);
	if (mesh.has_value())
		return mesh;
	
	mesh = LoadFromOBJ(path, device, type);
	if (mesh.has_value())
	{
		_loadedMeshes.emplace_back(path, mesh.value());
		return mesh;
	}

	return {};
}

OPT_SP_MESH Mesh::IsMeshLoaded(PATH_STR path)
{
	for (LOADED_MESHES::iterator i = _loadedMeshes.begin();
	     i != _loadedMeshes.end(); ++i)
	{
		if (i->first == path)
			return i->second;
	}

	return {};
}

OPT_SP_MESH Mesh::LoadFromOBJ(PATH_STR path, const ComPtr<ID3D11Device>& device, MeshType type)
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

			if (type == RightHanded)
				v2.y = 1.0f - v2.y;

			textureCoordinates.emplace_back(v2);
		}
		else if (objToken == "s")
		{
			stringStream >> mesh->SmoothShaded;
		}
		else if (objToken == "mtllib")
		{
			std::string name;
			stringStream >> name;

			std::string matPath = "Materials/" + name;
			std::optional<std::shared_ptr<::Material>> material = Material::MaterialFromPath(matPath);
			if (!material.has_value())
				mesh->Material = {};

			mesh->Material = material.value();
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

	mesh->Vertices.Elements = new Vertex[positionIndices.size()];
	mesh->Vertices.Length = positionIndices.size();
	for (int i = 0; i < positionIndices.size(); i ++)
	{
		mesh->Vertices.Elements[i].Position = positions[positionIndices[i]];
		mesh->Vertices.Elements[i].Normal = normals[normalIndices[i]];
		mesh->Vertices.Elements[i].TextureCoordinate = textureCoordinates[textCoordIndices[i]];
	}

	mesh->VertexIndices.Elements = new UINT[mesh->Vertices.Length];
	mesh->VertexIndices.Length = mesh->Vertices.Length;
	for (int i = 0; i < mesh->Vertices.Length; i ++)
		mesh->VertexIndices.Elements[i] = i;
	
		

	OPT_COM_BUFFER vertexBuffer = CreateVertexBuffer(path, mesh, device);
	if (!vertexBuffer.has_value())
		return {};
	mesh->VertexBuffer = vertexBuffer.value();

	OPT_COM_BUFFER indexBuffer = CreateIndexBuffer(path, mesh, device);
	if (!indexBuffer.has_value())
		return {};
	mesh->IndexBuffer = indexBuffer.value();

	return mesh;
}

OPT_COM_BUFFER Mesh::IsVertexBufferCreated(PATH_STR path)
{
	for (LOADED_BUFFERS::iterator i = _loadedVertexBuffers.begin();
		i != _loadedVertexBuffers.end(); ++i)
	{
		if (i->first == path)
			return i->second;
	}

	return {};
}

OPT_COM_BUFFER Mesh::IsIndexBufferCreated(PATH_STR path)
{
	for (LOADED_BUFFERS::iterator i = _loadedIndexBuffers.begin();
		i != _loadedIndexBuffers.end(); ++i)
	{
		if (i->first == path)
			return i->second;
	}

	return {};
}

void Mesh::ClearUnusedMeshes()
{
	bool removedMesh = false;

	do
	{
		removedMesh = false;

		for (auto mesh : _loadedMeshes)
		{
			if (mesh.second.use_count() == 2)
			{
				_loadedMeshes.remove_if([mesh, &removedMesh](const std::pair<std::string, SP_MESH>& m)
					{
						if (m.first == mesh.first)
							removedMesh = true;

						return m.first == mesh.first;
					});

				if (removedMesh)
					break;
			}
		}
	}
	while (removedMesh);
}

OPT_COM_BUFFER Mesh::CreateVertexBuffer(PATH_STR path, const SP_MESH& mesh, const ComPtr<ID3D11Device>& device)
{
	COM_BUFFER buffer = COM_BUFFER();

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * mesh->Vertices.Length;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceVertexData = { mesh->Vertices.Elements };

	HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &subresourceVertexData, &buffer);
	if (FAILED(hr))
		return {};

	_loadedVertexBuffers.emplace_back(path, buffer);

	return buffer;
}

OPT_COM_BUFFER Mesh::CreateIndexBuffer(PATH_STR path, const SP_MESH& mesh, const ComPtr<ID3D11Device>& device)
{
	COM_BUFFER buffer = COM_BUFFER();

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.ByteWidth = sizeof(*mesh->VertexIndices.Elements) * mesh->VertexIndices.Length; //size of the index data
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceIndexData = { mesh->VertexIndices.Elements };

	HRESULT hr = device->CreateBuffer(&indexBufferDesc, &subresourceIndexData, &buffer);
	if (FAILED(hr))
		return {};

	_loadedIndexBuffers.emplace_back(path, buffer);

	return buffer;
}