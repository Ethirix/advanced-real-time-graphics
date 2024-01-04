#include "RenderObject.h"

SP_MESH RenderObject::GetMesh()
{
	return _mesh;
}

SP_TEXTURES RenderObject::GetTextures()
{
	return _textures;
}

void RenderObject::SetTextures(const SP_TEXTURES& textures)
{
	_textures = textures;
}

void RenderObject::Draw(const ComPtr<ID3D11DeviceContext>& context, const ComPtr<ID3D11Buffer>& constantBuffer, 
	const ComPtr<ID3D11Buffer>& lightBuffer,
	BlinnConstantBuffer constantBufferData, LightBuffer lightBufferData)
{
	if (!_mesh)
		return;

	UINT stride = {sizeof(Vertex)};
    UINT offset =  0;

    context->IASetVertexBuffers(0, 1, _mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(_mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	if (_textures)
	{
		if (_textures->Diffuse != nullptr)
		{
			constantBufferData.HasDiffuseTexture = 1;
			context->PSSetShaderResources(0, 1, _textures->Diffuse.GetAddressOf());
		}
		if (_textures->Specular != nullptr)
		{
			constantBufferData.HasSpecularTexture = 1;
			context->PSSetShaderResources(1, 1, _textures->Specular.GetAddressOf());
		}
	}
	else
	{
		constantBufferData.HasDiffuseTexture = 0;
		constantBufferData.HasSpecularTexture = 0;
	}

	constantBufferData.AmbientMaterial = _mesh->Material->Ambient;
	constantBufferData.DiffuseMaterial = _mesh->Material->Diffuse;
	constantBufferData.SpecularMaterial = _mesh->Material->Specular;
	constantBufferData.SpecularExponent = _mesh->Material->SpecularExponent;

	D3D11_MAPPED_SUBRESOURCE subresource;
	D3D11_MAPPED_SUBRESOURCE subresource1;
	context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	constantBufferData.World = XMMatrixTranspose(XMLoadFloat4x4(&Transform->WorldMatrix));
	memcpy(subresource.pData, &constantBufferData, sizeof(constantBufferData));
	context->Unmap(constantBuffer.Get(), 0);

	context->Map(lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource1);
	memcpy(subresource1.pData, &lightBufferData, sizeof(lightBufferData));
	context->Unmap(lightBuffer.Get(), 0);

	context->DrawIndexed(_mesh->VertexIndices.Length, 0, 0);
}

void RenderObject::Draw(const ComPtr<ID3D11DeviceContext>& context, const ComPtr<ID3D11Buffer>& constantBuffer,
                        PhongConstantBuffer constantBufferData)
{
	if (!_mesh)
		return;

	UINT stride = {sizeof(Vertex)};
    UINT offset =  0;

    context->IASetVertexBuffers(0, 1, _mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(_mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	if (_textures)
	{
		if (_textures->Diffuse != nullptr)
		{
			constantBufferData.HasDiffuseTexture = 1;
			context->PSSetShaderResources(0, 1, _textures->Diffuse.GetAddressOf());
		}
		if (_textures->Specular != nullptr)
		{
			constantBufferData.HasSpecularTexture = 1;
			context->PSSetShaderResources(1, 1, _textures->Specular.GetAddressOf());
		}
	}
	else
	{
		constantBufferData.HasDiffuseTexture = 0;
		constantBufferData.HasSpecularTexture = 0;
	}

	constantBufferData.AmbientMaterial = _mesh->Material->Ambient;
	constantBufferData.DiffuseMaterial = _mesh->Material->Diffuse;
	constantBufferData.SpecularMaterial = _mesh->Material->Specular;
	constantBufferData.SpecularExponent = _mesh->Material->SpecularExponent;

	D3D11_MAPPED_SUBRESOURCE subresource;

	context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	constantBufferData.World = XMMatrixTranspose(XMLoadFloat4x4(&Transform->WorldMatrix));
	memcpy(subresource.pData, &constantBufferData, sizeof(constantBufferData));
	context->Unmap(constantBuffer.Get(), 0);

	context->DrawIndexed(_mesh->VertexIndices.Length, 0, 0);
}


void RenderObject::Update(const double deltaTime)
{
	//Transform->AddToRotation(0, deltaTime, 0);

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

void RenderObject::ClearExpiredChildren()
{
	auto newChildren = _children;
	for (unsigned i = 0; i < _children.size(); i++)
	{
		if (_children[i])
		{
			newChildren.emplace_back(_children[i]);
		}
	}

	_children = newChildren;
}