#include "AABBColliderComponent.h"

#include "CollisionResponse.h"
#include "Debug.h"
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
	Bounds bounds = GetBounds();
	tensor.m[0][0] = 1 / 12.0f * mass * (Maths::Pow(bounds.Extents.Y, 2) + Maths::Pow(bounds.Extents.Z, 2));
	tensor.m[1][1] = 1 / 12.0f * mass * (Maths::Pow(bounds.Extents.X, 2) + Maths::Pow(bounds.Extents.Z, 2));
	tensor.m[2][2] = 1 / 12.0f * mass * (Maths::Pow(bounds.Extents.X, 2) + Maths::Pow(bounds.Extents.Y, 2));

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
	Vector3 scale = GameObject.lock()->Transform->GetScale();

	bounds.Extents = {bounds.Extents.X * scale.X, bounds.Extents.Y * scale.Y, bounds.Extents.Z * scale.Z};
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

CollisionResponse AABBColliderComponent::AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

	if (!Intersects(collider))
		return {};

	Vector3 pos = GameObject.lock()->Transform->GetPosition();
	Vector3 closestPoint = ClosestPoint(collider->GameObject.lock()->Transform->GetPosition());
	Vector3 colliderClosestPoint = collider->ClosestPoint(pos);

	CollisionResponse response{};
	response.Collider = collider;
	response.Normal = Vector3::Normalise(pos - colliderClosestPoint);
	response.Transform = collider->GameObject.lock()->Transform;
	response.ClosestPointOnCollider = colliderClosestPoint;
	response.PhysicsComponent = collider->GameObject.lock()->GetComponent<PhysicsComponent>();
	response.PenetrationDepth = Vector3(colliderClosestPoint - closestPoint).Magnitude();

	return response;
}

CollisionResponse AABBColliderComponent::SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

	Vector3 pos = GameObject.lock()->Transform->GetPosition();
	Vector3 closestPoint = ClosestPoint(collider->GameObject.lock()->Transform->GetPosition());
	Vector3 colliderClosestPoint = collider->ClosestPoint(pos);

	Vector3 difference = colliderClosestPoint - closestPoint;
	float magSqr = difference.MagnitudeSqr();

	if (magSqr >= Maths::Pow(collider->GetRadius(), 2))
		return {};

	CollisionResponse response{};
	response.Collider = collider;
	response.Normal = Vector3::Normalise(pos - colliderClosestPoint);
	response.Transform = collider->GameObject.lock()->Transform;
	response.ClosestPointOnCollider = colliderClosestPoint;
	response.PhysicsComponent = collider->GameObject.lock()->GetComponent<PhysicsComponent>();
	response.PenetrationDepth = Maths::Pow(collider->GetRadius(), 2) - magSqr;

	return response;
}

CollisionResponse AABBColliderComponent::PlaneCollideCheck(std::shared_ptr<PlaneColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

	Bounds bounds = GetBounds();
	Vector3 planeNormal = collider->GetNormal();

	//https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
	float projectionRadius = bounds.Extents.X * std::abs(planeNormal.X) + bounds.Extents.Y * std::abs(planeNormal.Y)
		+ bounds.Extents.Z * std::abs(planeNormal.Z);

	float distance = planeNormal.Dot(bounds.Center) - collider->GetDistance();

	if (std::abs(distance) > projectionRadius)
		return {};

	Vector3 pos = GameObject.lock()->Transform->GetPosition();
	Vector3 colliderClosestPoint = collider->ClosestPoint(pos);

	CollisionResponse response{};
	response.Collider = collider;
	response.Normal = Vector3::Normalise(pos - colliderClosestPoint);
	response.Transform = collider->GameObject.lock()->Transform;
	response.ClosestPointOnCollider = colliderClosestPoint;
	response.PhysicsComponent = collider->GameObject.lock()->GetComponent<PhysicsComponent>();
	response.PenetrationDepth = std::abs(Vector3(colliderClosestPoint - pos).Magnitude() - bounds.Extents.Y);

	return response;
}
