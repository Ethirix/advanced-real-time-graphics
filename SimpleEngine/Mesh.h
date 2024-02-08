#pragma once
#include <d3d11.h>

#include "Material.h"
#include "SafePointerArray.h"
#include "Vector3.h"
#include "Vertex.h"

class Mesh
{
public:
	std::string Name = "";
	SafePointerArray<struct Vertex> Vertices{};
	SafePointerArray<UINT> VertexIndices{};
	bool SmoothShaded = true;

	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer = {};
	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer = {};

	Vector3 Min;
	Vector3 Max;
};