#pragma once

#include <list>
#include <memory>
#include <string>

#include "Mesh.h"

#define SP_MESH std::shared_ptr<Mesh>
#define COM_BUFFER Microsoft::WRL::ComPtr<ID3D11Buffer>
#define LOADED_MESHES std::list<std::pair<std::string, SP_MESH>>
#define LOADED_BUFFERS std::list<std::pair<std::string, COM_BUFFER>>

struct CreatedMeshes
{
	LOADED_MESHES Meshes{};
	LOADED_BUFFERS VertexBuffers{};
	LOADED_BUFFERS IndexBuffers{};
};
