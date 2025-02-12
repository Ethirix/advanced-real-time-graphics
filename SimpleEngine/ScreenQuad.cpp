#include "ScreenQuad.h"

#include "DataStore.h"
#include "SafePointerArray.h"
#include "Vertex.h"

ScreenQuad::ScreenQuad(const Microsoft::WRL::ComPtr<ID3D11Device>& device, float size, DirectX::XMFLOAT2 offset)
{
	_mesh = {};

	_mesh.Vertices.Elements = new struct Vertex[4];
	_mesh.Vertices.Length = 4;

	DirectX::XMFLOAT2 topLeftPos = { -1 + offset.x * 2, 1 - offset.y * 2 };
	
	_mesh.Vertices.Elements[0].Position = { topLeftPos.x, topLeftPos.y - size * 2.0f, 0.0f };
	_mesh.Vertices.Elements[1].Position = { topLeftPos.x, topLeftPos.y,  0.0f };
	_mesh.Vertices.Elements[2].Position = { topLeftPos.x + size * 2.0f, topLeftPos.y,   0.0f };
	_mesh.Vertices.Elements[3].Position = { topLeftPos.x + size * 2.0f, topLeftPos.y - size * 2.0f,  0.0f };

	_mesh.Vertices.Elements[0].Normal = { 0.0f, 1.0f, 0.0f };
	_mesh.Vertices.Elements[1].Normal = { 0.0f, 1.0f, 0.0f };
	_mesh.Vertices.Elements[2].Normal = { 0.0f, 1.0f, 0.0f };
	_mesh.Vertices.Elements[3].Normal = { 0.0f, 1.0f, 0.0f };

	_mesh.Vertices.Elements[0].TextureCoordinate = { 0.0f, 1.0f };
	_mesh.Vertices.Elements[1].TextureCoordinate = { 0.0f, 0.0f };
	_mesh.Vertices.Elements[2].TextureCoordinate = { 1.0f, 0.0f };
	_mesh.Vertices.Elements[3].TextureCoordinate = { 1.0f, 1.0f };

	_mesh.VertexIndices.Elements = new UINT[6];
	_mesh.VertexIndices.Length = 6;

	_mesh.VertexIndices.Elements[0] = 0;
	_mesh.VertexIndices.Elements[1] = 1;
	_mesh.VertexIndices.Elements[2] = 2;
	_mesh.VertexIndices.Elements[3] = 0;
	_mesh.VertexIndices.Elements[4] = 2;
	_mesh.VertexIndices.Elements[5] = 3;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer{};

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(struct Vertex) * _mesh.Vertices.Length;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData = { .pSysMem = _mesh.Vertices.Elements };
	device->CreateBuffer(&bufferDesc, &subresourceData, &vertexBuffer);

	bufferDesc.ByteWidth = sizeof(UINT) * _mesh.VertexIndices.Length;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subresourceData = { .pSysMem = _mesh.VertexIndices.Elements };
	device->CreateBuffer(&bufferDesc, &subresourceData, &indexBuffer);

	_mesh.VertexBuffer = vertexBuffer;
	_mesh.IndexBuffer = indexBuffer;
}

void ScreenQuad::Draw(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
{
	UINT stride = { sizeof(struct Vertex) };
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, _mesh.VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(_mesh.IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, offset);

	context->DrawIndexed(_mesh.VertexIndices.Length, 0, 0);
}
