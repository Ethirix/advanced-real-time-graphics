#pragma once
#include <memory>

#include "ComponentBase.h"
#include "Mesh.h"
#include "MeshType.h"
#include "Textures.h"

#define STR const std::string&

class MeshComponent final : public ComponentBase
{
public:
	explicit MeshComponent(WP_GAMEOBJECT owningGameObject, 
		const Microsoft::WRL::ComPtr<ID3D11Device>& device, 
		STR meshPath, STR materialPath, STR vertexShaderPath, 
		STR pixelShaderPath, MeshType meshType, 
		std::pair<STR, int> diffuse, std::pair<STR, int> specular, 
		std::pair<STR, int> normal, std::pair<STR, int> displacement,
		STR stencil);

	void FixedUpdate(double fixedDeltaTime) override {};
	void Update(double deltaTime) override {};
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

	std::shared_ptr<Mesh> Mesh;
	std::shared_ptr<Material> Material{};
	std::shared_ptr<Textures> Textures{};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencil = nullptr;
};
