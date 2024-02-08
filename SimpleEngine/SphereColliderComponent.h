#pragma once
#include <nlohmann/json.hpp>

#include "ColliderComponent.h"

class SphereColliderComponent : public ColliderComponent
{
public:
	SphereColliderComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json);

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}
	void FixedUpdate(double fixedDeltaTime) override;
	void Update(double deltaTime) override {}

	[[nodiscard]] float GetRadius();

protected:
	bool SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider) override;

private:
	float _radius = 0;
};
