#include "MeshComponent.h"

#include "Buffers.h"
#include "DataStore.h"
#include "Factory.h"
#include "GameObject.h"

MeshComponent::MeshComponent(const std::weak_ptr<class GameObject>& owningGameObject, const Microsoft::WRL::ComPtr<ID3D11Device>& device,
                             STR meshPath, STR materialPath,
                             STR vertexShaderPath, STR pixelShaderPath, 
                             MeshType meshType, std::pair<STR, int> diffuse, std::pair<STR, int> specular, 
							 std::pair<STR, int> normal, std::pair<STR, int> displacement, STR stencil)
							 : ComponentBase(owningGameObject)
{
    if (auto mesh = Factory::CreateMesh(meshPath, meshType, device); mesh.has_value())
        Mesh = mesh.value();
    else
        Mesh = nullptr;

    if (auto material = Factory::CreateMaterial(materialPath); material.has_value())
        Material = material.value();
    else
        Material = std::make_shared<class Material>();

	Factory::LoadVertexShader(vertexShaderPath, Material);
	Factory::LoadPixelShader(pixelShaderPath, Material);

    Textures = std::make_shared<struct Textures>();
    if (auto diffuseResource = Factory::CreateTexture(diffuse.first, device); diffuseResource.has_value())
        Textures->Diffuse = {diffuse.first, diffuseResource.value(), diffuse.second};
    if (auto specularResource = Factory::CreateTexture(specular.first, device); specularResource.has_value())
        Textures->Specular = {specular.first, specularResource.value(), specular.second};
    if (auto displacementResource = Factory::CreateTexture(displacement.first, device); displacementResource.has_value())
        Textures->Displacement = {displacement.first, displacementResource.value(), displacement.second};
    if (auto normalResource = Factory::CreateTexture(normal.first, device); normalResource.has_value())
        Textures->Normal = {normal.first, normalResource.value(), normal.second};

	if (auto state = DataStore::DepthStencilStates.Retrieve(stencil); state.has_value())
        DepthStencil = state.value();
}

void MeshComponent::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
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

	Buffers::CBTextures.BufferData.HasDiffuseTexture = Textures->Diffuse.Resource ? true : false;
    Buffers::CBTextures.BufferData.HasSpecularTexture = Textures->Specular.Resource ? true : false;

    if (Textures->Diffuse.Slot != -1)
		context->PSSetShaderResources(Textures->Diffuse.Slot, 1, Textures->Diffuse.Resource.GetAddressOf());
    if (Textures->Specular.Slot != -1)
		context->PSSetShaderResources(Textures->Specular.Slot, 1, Textures->Specular.Resource.GetAddressOf());

    Buffers::CBMaterial.BufferData.Ambient = Material ? Material->Ambient : DirectX::XMFLOAT4{};
    Buffers::CBMaterial.BufferData.Diffuse = Material ? Material->Diffuse : DirectX::XMFLOAT4{};
    Buffers::CBMaterial.BufferData.Specular = Material ? Material->Specular : DirectX::XMFLOAT4{};
    Buffers::CBMaterial.BufferData.SpecularExponent = Material ? Material->SpecularExponent : 0;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState = nullptr;
    context->OMGetDepthStencilState(depthStencilState.GetAddressOf(), nullptr);
    if (depthStencilState != DepthStencil)
        context->OMSetDepthStencilState(DepthStencil.Get(), 0);


    D3D11_MAPPED_SUBRESOURCE objectCameraData, materialData, texturesData, lightingData;
	Buffers::CBObjectCameraData.BufferData.World = XMMatrixTranspose(XMLoadFloat4x4(&GameObject.lock()->Transform->WorldMatrix));
    context->Map(Buffers::CBObjectCameraData.Buffer.Get(), 
        0, D3D11_MAP_WRITE_DISCARD, 0, &objectCameraData);
    memcpy(objectCameraData.pData, &Buffers::CBObjectCameraData.BufferData,
           sizeof(Buffers::CBObjectCameraData.BufferData));
    context->Unmap(Buffers::CBObjectCameraData.Buffer.Get(), 0);

    context->Map(Buffers::CBMaterial.Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &materialData);
    memcpy(materialData.pData, &Buffers::CBMaterial.BufferData, sizeof(Buffers::CBMaterial.BufferData));
    context->Unmap(Buffers::CBMaterial.Buffer.Get(), 0);

    context->Map(Buffers::CBTextures.Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &texturesData);
    memcpy(texturesData.pData, &Buffers::CBTextures.BufferData, sizeof(Buffers::CBTextures.BufferData));
    context->Unmap(Buffers::CBTextures.Buffer.Get(), 0);

    context->Map(Buffers::CBLighting.Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &lightingData);
    memcpy(lightingData.pData, &Buffers::CBLighting.BufferData, sizeof(Buffers::CBLighting.BufferData));
    context->Unmap(Buffers::CBLighting.Buffer.Get(), 0);

    context->DrawIndexed(Mesh->VertexIndices.Length, 0, 0);
}
