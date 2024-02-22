#pragma once
#include <nlohmann/json.hpp>

#include "ColliderComponent.h"
#include "Vector3.h"

class SphereColliderComponent : public ColliderComponent
{
public:
	SphereColliderComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json);

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}
	//void FixedUpdate(double fixedDeltaTime) override;
	void Update(double deltaTime) override {}

	[[nodiscard]] float GetRadius();
	[[nodiscard]] bool IsPointInsideSphere(Vector3 point);
	[[nodiscard]] Vector3 ClosestPoint(Vector3 point) override;
	[[nodiscard]] DirectX::XMFLOAT3X3 GetInertiaTensor(float mass) override;

protected:
	CollisionResponse SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider) override;
	CollisionResponse AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider) override;
	CollisionResponse PlaneCollideCheck(std::shared_ptr<PlaneColliderComponent> collider) override;
private:
	float _radius = 0;
};
