#pragma once
#include <nlohmann/json.hpp>

#include "LightComponent.h"

class SpotLightComponent :
    public LightComponent
{
public:
	~SpotLightComponent() override = default;
	explicit SpotLightComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json);

	void Update(double deltaTime) override {}
	void FixedUpdate(double fixedDeltaTime) override {}
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}

    float InnerAngle;
	float OuterAngle;
	DirectX::XMFLOAT3 Direction;
};

