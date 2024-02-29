#pragma once
#include <nlohmann/json.hpp>

#include "ComponentBase.h"
#include "LightData.h"

class LightComponent :
    public ComponentBase
{
public:
	explicit LightComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json);

	void Update(double deltaTime) override;
	void FixedUpdate(double fixedDeltaTime) override {}
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}

	LightData Light;
};