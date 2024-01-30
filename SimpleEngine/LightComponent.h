#pragma once
#include "ComponentBase.h"
#include "LightData.h"

class LightComponent :
    public ComponentBase
{
public:
	LightComponent(WP_GAMEOBJECT owningGameObject, const LightData& lightData) : ComponentBase(owningGameObject), Light(lightData)
	{ }

	void Update(double deltaTime) override;
	void FixedUpdate(double fixedDeltaTime) override {}
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}

	LightData Light;
};