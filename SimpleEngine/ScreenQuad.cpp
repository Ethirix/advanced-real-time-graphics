#include "ScreenQuad.h"

ScreenQuad::ScreenQuad(const Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	Vertex* v = new Vertex[4];
	
	v[0].Position = { -1.0f, -1.0f, 0.0f };
	v[1].Position = { -1.0f, 1.0f, 0.0f };
	v[2].Position = { 1.0f, 1.0f, 0.0f };
	v[3].Position = { 1.0f, -1.0f, 0.0f };

	v[0].Normal = { 0-1.0f, 1.0f, 0.0f };
	v[1].Normal = { 0-1.0f, 1.0f, 0.0f };
	v[2].Normal = { 0-1.0f, 1.0f, 0.0f };
	v[3].Normal = { 0-1.0f, 1.0f, 0.0f };

	v[0].TextureCoordinate = { 0.0f, 1.0f };
	v[1].TextureCoordinate = { 0.0f, 0.0f };
	v[2].TextureCoordinate = { 1.0f, 0.0f };
	v[3].TextureCoordinate = { 1.0f, 1.0f };

	UINT i[] = {0, 1, 2, 0, 2, 3};

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer{};

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(Vertex) * 4;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData = { v };
	device->CreateBuffer(&bufferDesc, &subresourceData, &vertexBuffer);

	bufferDesc.ByteWidth = sizeof(UINT) * 6;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subresourceData = { i };
	device->CreateBuffer(&bufferDesc, &subresourceData, &indexBuffer);


	_mesh = {
		.Name = "SSQ",
		.Vertices = {v, 4},
		.VertexIndices = {i, 6},
		.SmoothShaded = false,
		.VertexBuffer = vertexBuffer,
		.IndexBuffer = indexBuffer,
		.Bounds = {}
	};
}
