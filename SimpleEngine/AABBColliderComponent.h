#pragma once
#include <nlohmann/json.hpp>

#include "Bounds.h"
#include "ColliderComponent.h"

class AABBColliderComponent : public ColliderComponent
{
public:
	explicit AABBColliderComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json);

	void Update(double deltaTime) override {}
	void FixedUpdate(double fixedDeltaTime) override;
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}

	[[nodiscard]] Bounds GetBounds();
	[[nodiscard]] Vector3 GetClosestPoint(Vector3 point);
	[[nodiscard]] bool Intersects(const Bounds& other);
protected:
	bool SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider) override;
	bool AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider) override;

private:
	//USE GETTER!
	Bounds _bounds = {};
	bool _useMeshBounds = true;
};
