#pragma once
#include <nlohmann/json.hpp>

#include "LightComponent.h"

class DirectionalLightComponent :
    public LightComponent
{
public:
    ~DirectionalLightComponent() override = default;
	explicit DirectionalLightComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json);

	void Update(double deltaTime) override {}
	void FixedUpdate(double fixedDeltaTime) override {}
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}

	DirectX::XMFLOAT4 Direction;
};

