#pragma once
#include <nlohmann/json.hpp>

#include "ColliderComponent.h"
#include "Vector3.h"

class SphereColliderComponent : public ColliderComponent
{
public:
	SphereColliderComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json);

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}
	void FixedUpdate(double fixedDeltaTime) override;
	void Update(double deltaTime) override {}

	[[nodiscard]] float GetRadius();
	[[nodiscard]] bool IsPointInsideSphere(Vector3 point);

protected:
	bool SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider) override;
	bool AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider) override;
	bool PlaneCollideCheck(std::shared_ptr<PlaneColliderComponent> collider) override;
private:
	float _radius = 0;
};
