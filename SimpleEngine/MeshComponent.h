#pragma once
#include <memory>

#include "ComponentBase.h"
#include "Mesh.h"
#include "MeshType.h"

#define PATH const std::string&

class MeshComponent final : public ComponentBase
{
public:
	explicit MeshComponent(WP_GAMEOBJECT owningGameObject, 
		const Microsoft::WRL::ComPtr<ID3D11Device>& device, 
		PATH meshPath, PATH materialPath, PATH vertexShaderPath, 
		PATH pixelShaderPath, MeshType meshType);

	void FixedUpdate(double fixedDeltaTime) override {};
	void Update(double deltaTime) override {};
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

	std::shared_ptr<Mesh> Mesh;
	std::shared_ptr<Material> Material{};
};
