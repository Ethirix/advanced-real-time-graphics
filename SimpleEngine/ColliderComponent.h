#pragma once
#include "ColliderType.h"
#include "ComponentBase.h"
#include "Vector3.h"

class PlaneColliderComponent;
class AABBColliderComponent;
class SphereColliderComponent;
class ColliderComponent : public ComponentBase
{
public:
	std::shared_ptr<ColliderComponent> GetColliderPtr() { return std::static_pointer_cast<ColliderComponent>(shared_from_this()); }

	explicit ColliderComponent(WP_GAMEOBJECT owningGameObject);

	ColliderType Type = COLLIDER_NONE;
	bool Collideable = true;

	void Update(double deltaTime) override = 0;
	void FixedUpdate(double fixedDeltaTime) override;
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override = 0;

	virtual Vector3 ClosestPoint(Vector3 point) = 0;

	bool CollidesWith(const std::shared_ptr<ColliderComponent>& collider);

	void RunCollisionResponse(double fixedDeltaTime);

protected:
	virtual bool SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider) = 0;
	virtual bool AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider) = 0;
	virtual bool PlaneCollideCheck(std::shared_ptr<PlaneColliderComponent> collider) = 0;
};
