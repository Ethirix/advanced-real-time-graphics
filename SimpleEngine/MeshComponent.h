#pragma once
#include <memory>
#include <nlohmann/json.hpp>

#include "ComponentBase.h"
#include "Mesh.h"
#include "Textures.h"

#define STR const std::string&

class MeshComponent final : public ComponentBase
{
public:
	explicit MeshComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json, 
		const Microsoft::WRL::ComPtr<ID3D11Device>& device);

	void FixedUpdate(double fixedDeltaTime) override {};
	void Update(double deltaTime) override {};
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

	std::shared_ptr<Mesh> Mesh;
	std::shared_ptr<Material> Material{};
	std::shared_ptr<Textures> Textures{};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencil = nullptr;
};
