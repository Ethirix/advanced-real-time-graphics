#pragma once
#include <nlohmann/json.hpp>

#include "ColliderComponent.h"
#include "Vector3.h"

class PlaneColliderComponent : public ColliderComponent
{
public:
	explicit PlaneColliderComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json);

	void Update(double deltaTime) override {}
	//void FixedUpdate(double fixedDeltaTime) override;
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}

	void SetNormal(Vector3 normal);
	void SetDistance(float distance);

	[[nodiscard]] Vector3 GetNormal();
	[[nodiscard]] float GetDistance();

	[[nodiscard]] bool Intersects(const std::shared_ptr<PlaneColliderComponent>& plane);
	[[nodiscard]] bool PointOnPlane(Vector3 point);
	[[nodiscard]] Vector3 ClosestPoint(Vector3 point) override;
	[[nodiscard]] DirectX::XMFLOAT3X3 GetInertiaTensor(float mass) override;

protected:
	bool SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider) override;
	CollisionResponse AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider) override;
	bool PlaneCollideCheck(std::shared_ptr<PlaneColliderComponent> collider) override;

private:
	///USE GETTER!
	Vector3 _normal = {};
	float _distance = 0;
};
