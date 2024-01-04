#include "Skybox.h"

void Skybox::Draw(const ComPtr<ID3D11DeviceContext>& context, 
	const ComPtr<ID3D11Buffer>& skyboxBuffer, 
	SkyboxCBuffer& skyboxBufferData,
	const std::shared_ptr<Camera>& camera)
{
	if (!_mesh)
		return;

	UINT stride = {sizeof(Vertex)};
    UINT offset =  0;

    context->IASetVertexBuffers(0, 1, _mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(_mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->PSSetShaderResources(0, 1, _textures->Diffuse.GetAddressOf());
	D3D11_MAPPED_SUBRESOURCE subresource;

	skyboxBufferData.Projection = XMLoadFloat4x4(&camera->GetProjection());
    skyboxBufferData.View = XMLoadFloat4x4(&camera->GetView());
	skyboxBufferData.World = XMMatrixTranspose(XMLoadFloat4x4(&Transform->WorldMatrix));

	context->Map(skyboxBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	memcpy(subresource.pData, &skyboxBufferData, sizeof(skyboxBufferData));
	context->Unmap(skyboxBuffer.Get(), 0);

	context->DrawIndexed(_mesh->VertexIndices.Length, 0, 0);
}

void Skybox::Update(double deltaTime)
{
	Transform->AddToPosition(0, deltaTime, 0);

	const DirectX::XMFLOAT3 rotation = Transform->GetRotation();

	DirectX::XMMATRIX matrix = DirectX::XMMatrixScaling(
			Transform->GetScale().x,
			Transform->GetScale().y,
			Transform->GetScale().z)
		* DirectX::XMMatrixRotationX(rotation.x)
		* DirectX::XMMatrixRotationY(rotation.y)
		* DirectX::XMMatrixRotationZ(rotation.z)
		* DirectX::XMMatrixTranslation(
			Transform->GetPosition().x,
			Transform->GetPosition().y,
			Transform->GetPosition().z);

	if (_parent)
		matrix *= XMLoadFloat4x4(&_parent->Transform->WorldMatrix);

	XMStoreFloat4x4(&Transform->WorldMatrix, matrix);
}
