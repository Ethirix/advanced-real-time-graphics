#pragma once
#include <d3d11.h>
#include <memory>

#include "Material.h"
#include "SafePointerArray.h"
#include "Vertex.h"

class Mesh
{
public:
	std::string Name;
	SafePointerArray<class Vertex> Vertices{};
	SafePointerArray<UINT> VertexIndices{};
	std::shared_ptr<Material> Material{};
	bool SmoothShaded;

	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
};