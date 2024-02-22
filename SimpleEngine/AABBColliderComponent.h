#pragma once
#include <nlohmann/json.hpp>

#include "Bounds.h"
#include "ColliderComponent.h"

class AABBColliderComponent : public ColliderComponent
{
public:
	explicit AABBColliderComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json);

	void Update(double deltaTime) override {}
	//void FixedUpdate(double fixedDeltaTime) override;
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}

	[[nodiscard]] Vector3 ClosestPoint(Vector3 point) override;
	[[nodiscard]] DirectX::XMFLOAT3X3 GetInertiaTensor(float mass) override;

	[[nodiscard]] Bounds GetBounds();

protected:
	CollisionResponse SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider) override;
	CollisionResponse AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider) override;
	CollisionResponse PlaneCollideCheck(std::shared_ptr<PlaneColliderComponent> collider) override;

private:
	bool Intersects(std::shared_ptr<AABBColliderComponent> collider);

	//USE GetBounds()!
	Bounds _noneMeshBounds = {};
	bool _useMeshBounds = true;
};
