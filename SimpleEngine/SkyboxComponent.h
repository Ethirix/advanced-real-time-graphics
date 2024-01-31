#pragma once
#include "ComponentBase.h"
#include "Mesh.h"
#include "MeshType.h"
#include "Textures.h"

#define PATH const std::string&

class SkyboxComponent final : public ComponentBase
{
public:
	explicit SkyboxComponent(WP_GAMEOBJECT owningGameObject, 
		const Microsoft::WRL::ComPtr<ID3D11Device>& device,
		PATH meshPath, MeshType meshType, PATH texturePath);

	void FixedUpdate(double fixedDeltaTime) override {}
	void Update(double deltaTime) override {}
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

	std::shared_ptr<Mesh> Mesh{};
	std::shared_ptr<Material> Material{};
	std::shared_ptr<Textures> Textures{};
};
