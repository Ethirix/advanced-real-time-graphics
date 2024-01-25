#include "MeshComponent.h"

#include "Buffers.h"
#include "Factory.h"
#include "GameObject.h"

MeshComponent::MeshComponent(const std::weak_ptr<class GameObject>& owningGameObject, const Microsoft::WRL::ComPtr<ID3D11Device>& device,
                             PATH meshPath, PATH materialPath,
                             PATH vertexShaderPath, PATH pixelShaderPath, 
                             MeshType meshType) : ComponentBase(owningGameObject)
{
    if (auto mesh = Factory::CreateMesh(meshPath, meshType, device); mesh.has_value())
        Mesh = mesh.value();
    else
        Mesh = nullptr;

    if (auto material = Factory::CreateMaterial(materialPath); material.has_value())
        Material = material.value();
    else
        Material = nullptr;
        
    if (Material)
    {
        Factory::LoadVertexShader(vertexShaderPath, Material);
        Factory::LoadPixelShader(pixelShaderPath, Material);
    }
}

void MeshComponent::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	if (!Mesh)
        return;

    UINT stride = { sizeof(class Vertex) };
    UINT offset = 0;

    context->IASetVertexBuffers(0, 1, Mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(Mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, offset);

    context->VSSetShader(Material->Shader.VertexShader.Shader.Get(),
        nullptr, 0);
    context->PSSetShader(Material->Shader.PixelShader.Shader.Get(),
        nullptr, 0);
    
    //temporary
    Buffers::CBTextures.BufferData.HasDiffuseTexture = false;
    Buffers::CBTextures.BufferData.HasSpecularTexture = false;

    if(Material)
    {
        Buffers::CBMaterial.BufferData.Ambient = Material->Ambient;
        Buffers::CBMaterial.BufferData.Diffuse = Material->Diffuse;
        Buffers::CBMaterial.BufferData.Specular = Material->Specular;
        Buffers::CBMaterial.BufferData.SpecularExponent = Material->SpecularExponent;
    }
    else
    {
        Buffers::CBMaterial.BufferData.Ambient = {};
        Buffers::CBMaterial.BufferData.Diffuse = {};
        Buffers::CBMaterial.BufferData.Specular = {};
        Buffers::CBMaterial.BufferData.SpecularExponent = {};
    }
    

    D3D11_MAPPED_SUBRESOURCE objectCameraData, materialData, texturesData, lightingData;

    context->Map(Buffers::CBObjectCameraData.Buffer.Get(), 
        0, D3D11_MAP_WRITE_DISCARD, 0, &objectCameraData);
    Buffers::CBObjectCameraData.BufferData.World = XMMatrixTranspose(
	    XMLoadFloat4x4(&GameObject.lock().get()->Transform->WorldMatrix));
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
