#include "AABBColliderComponent.h"

#include "CollisionResponse.h"
#include "GameObject.h"
#include "Maths.h"
#include "MeshComponent.h"
#include "SceneGraph.h"

#include "SphereColliderComponent.h"
#include "PlaneColliderComponent.h"

AABBColliderComponent::AABBColliderComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json)
	: ColliderComponent(owningGameObject)
{
	Type = COLLIDER_AABB;

	nlohmann::json min = json["Bounds"]["Min"];
	nlohmann::json max = json["Bounds"]["Max"];
	_noneMeshBounds.Min = {min["X"], min["Y"], min["Z"]};
	_noneMeshBounds.Max = {max["X"], max["Y"], max["Z"]};
	_noneMeshBounds.Extents = _noneMeshBounds.Max;
	_noneMeshBounds.Size = _noneMeshBounds.Extents * 2;
	_noneMeshBounds.Center = Vector3::Zero();

	_useMeshBounds = json["UseMeshBounds"];
	Collideable = json["Collideable"];
}

Vector3 AABBColliderComponent::ClosestPoint(Vector3 point)
{
	auto [Center, Min, Max, Extents, Size] = GetBounds();
	Vector3 closestPoint = {
		Maths::Max(Min.X, Maths::Min(point.X, Max.X)),
		Maths::Max(Min.Y, Maths::Min(point.Y, Max.Y)),
		Maths::Max(Min.Z, Maths::Min(point.Z, Max.Z))
	};

	return closestPoint;
}

DirectX::XMFLOAT3X3 AABBColliderComponent::GetInertiaTensor(float mass)
{
	DirectX::XMFLOAT3X3 tensor {};
	auto [Center, Min, Max] = GetBounds();
	Vector3 extents = { Max.X, Max.Y, Min.Z };
	tensor.m[0][0] = 1 / 12.0f * mass * (Maths::Pow(extents.Y, 2) + Maths::Pow(extents.Z, 2));
	tensor.m[1][1] = 1 / 12.0f * mass * (Maths::Pow(extents.X, 2) + Maths::Pow(extents.Z, 2));
	tensor.m[2][2] = 1 / 12.0f * mass * (Maths::Pow(extents.X, 2) + Maths::Pow(extents.Y, 2));

	return tensor;
}

Bounds AABBColliderComponent::GetBounds()
{
	Bounds bounds = _noneMeshBounds;

	if (_useMeshBounds)
	{
		if (auto optionalMesh = GameObject.lock()->TryGetComponent<MeshComponent>(); 
			optionalMesh.has_value())
			bounds = optionalMesh.value().lock()->Mesh->Bounds;
	}

	Vector3 pos = GameObject.lock()->Transform->GetPosition();

	bounds.Max = pos + bounds.Extents;
	bounds.Min = pos - bounds.Extents;
	bounds.Center = pos;

	return bounds;
}

bool AABBColliderComponent::Intersects(const std::shared_ptr<AABBColliderComponent>& collider)
{
	Bounds thisBounds = GetBounds();
	Bounds colliderBounds = collider->GetBounds();
	return (thisBounds.Min.X <= colliderBounds.Max.X && thisBounds.Max.X >= colliderBounds.Min.X) &&
		   (thisBounds.Min.Y <= colliderBounds.Max.Y && thisBounds.Max.Y >= colliderBounds.Min.Y) &&
		   (thisBounds.Min.Z <= colliderBounds.Max.Z && thisBounds.Max.Z >= colliderBounds.Min.Z);
}

//CollisionResponse response{};
//response.Collider = collider;
//response.Normal = closestPoint - collider->ClosestPoint(pos);
//response.Transform = collider->GameObject.lock()->Transform;
//response.ClosestPointOnCollider = collider->ClosestPoint(pos);
//response.PhysicsComponent = physComp;
//response.PenetrationDepth = Vector3(closestPoint - colliderClosestPoint).Magnitude();

//return response;

CollisionResponse AABBColliderComponent::AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

	if (!Intersects(collider))
		return {};

	//Do Collision Response
	return {};
}

CollisionResponse AABBColliderComponent::SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

}

CollisionResponse AABBColliderComponent::PlaneCollideCheck(std::shared_ptr<PlaneColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

}
