#pragma once
#include <memory>

#include "ComponentBase.h"
#include "Mesh.h"
#include "MeshType.h"

#define PATH const std::string&

class MeshComponent final : public ComponentBase
{
public:
	explicit MeshComponent(class GameObject* owningGameObject, const Microsoft::WRL::ComPtr<ID3D11Device>& device, PATH meshPath, PATH materialPath, PATH vertexShaderPath, PATH pixelShaderPath, MeshType meshType);

	void FixedUpdate() override;
	void Update() override;

	std::shared_ptr<Mesh> Mesh;
	std::shared_ptr<Material> Material{};
};
