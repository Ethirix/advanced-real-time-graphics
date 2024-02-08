#include "AABBColliderComponent.h"

#include "GameObject.h"
#include "MeshComponent.h"

AABBColliderComponent::AABBColliderComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json)
	: ColliderComponent(owningGameObject)
{
	Type = COLLIDER_AABB;

	nlohmann::json min = json["Min"];
	nlohmann::json max = json["Max"];
	_bounds.Min = {min["X"], min["Y"], min["Z"]};
	_bounds.Max = {max["X"], max["Y"], max["Z"]};
	_bounds.Center = Vector3::Zero();

	_useMeshBounds = json["UseMeshBounds"];
	Collideable = json["Collideable"];
}


Bounds AABBColliderComponent::GetBounds()
{
	if (_useMeshBounds)
	{
		if (auto optionalMesh = GameObject.lock()->TryGetComponent<MeshComponent>(); optionalMesh.has_value())
			return optionalMesh.value().lock()->GetBounds();
	}

	Bounds bounds = _bounds;
	bounds.Center = GameObject.lock()->Transform->GetPosition();

	return bounds;
}