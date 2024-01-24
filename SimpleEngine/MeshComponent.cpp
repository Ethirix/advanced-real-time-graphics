#include "MeshComponent.h"

#include "Factory.h"

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
