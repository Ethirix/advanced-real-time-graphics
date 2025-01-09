#pragma once
#include <nlohmann/json.hpp>

#include "LightComponent.h"

class PointLightComponent :
    public LightComponent
{
public:
	~PointLightComponent() override = default;
	explicit PointLightComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json);

	void Update(double deltaTime) override {}
	void FixedUpdate(double fixedDeltaTime) override {}
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}

	float ConstantAttenuation;
	float LinearAttenuation;
	float QuadraticAttenuation;
	float LightRadius;
};