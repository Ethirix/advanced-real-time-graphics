#include "SkyboxComponent.h"

#include "Buffers.h"
#include "Factory.h"
#include "GameObject.h"

SkyboxComponent::SkyboxComponent(WP_GAMEOBJECT owningGameObject, 
                                 const Microsoft::WRL::ComPtr<ID3D11Device>& device, 
                                 PATH meshPath, MeshType meshType, PATH texturePath) : ComponentBase(owningGameObject)
{
	if (auto mesh = Factory::CreateMesh(meshPath, meshType, device); mesh.has_value())
        Mesh = mesh.value();
    else
        Mesh = nullptr;

    Textures = std::make_shared<struct Textures>();
    if (auto diffuse = Factory::CreateTexture(texturePath, device); diffuse.has_value())
        Textures->Diffuse = {texturePath, diffuse.value()};
}

void SkyboxComponent::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	if (!Mesh)
        return;

    UINT stride = { sizeof(struct Vertex) };
    UINT offset = 0;

    context->IASetVertexBuffers(0, 1, Mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(Mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, offset);

    context->VSSetShader(Material->Shader.VertexShader.Shader.Get(),
        nullptr, 0);
    context->PSSetShader(Material->Shader.PixelShader.Shader.Get(),
        nullptr, 0);

    context->PSSetShaderResources(Textures->Diffuse.Slot, 1, Textures->Diffuse.Resource.GetAddressOf());
    context->PSSetShaderResources(Textures->Specular.Slot, 1, Textures->Specular.Resource.GetAddressOf());
    D3D11_MAPPED_SUBRESOURCE objectCameraData;

	Buffers::CBObjectCameraData.BufferData.World = XMMatrixTranspose(XMLoadFloat4x4(&GameObject.lock()->Transform->WorldMatrix));
    context->Map(Buffers::CBObjectCameraData.Buffer.Get(), 
        0, D3D11_MAP_WRITE_DISCARD, 0, &objectCameraData);
    memcpy(objectCameraData.pData, &Buffers::CBObjectCameraData.BufferData,
           sizeof(Buffers::CBObjectCameraData.BufferData));
    context->Unmap(Buffers::CBObjectCameraData.Buffer.Get(), 0);


    context->DrawIndexed(Mesh->VertexIndices.Length, 0, 0);
}
