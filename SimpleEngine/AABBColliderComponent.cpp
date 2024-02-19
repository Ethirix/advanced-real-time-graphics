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
	_bounds.Min = {min["X"], min["Y"], min["Z"]};
	_bounds.Max = {max["X"], max["Y"], max["Z"]};
	_bounds.Center = Vector3::Zero();

	_useMeshBounds = json["UseMeshBounds"];
	Collideable = json["Collideable"];
}

Vector3 AABBColliderComponent::ClosestPoint(Vector3 point)
{
	auto [Center, Min, Max] = GetBounds();
	Vector3 boxPoint = {
		Maths::Max(Center.X + Min.X, Maths::Min(point.X, Center.X + Max.X)),
		Maths::Max(Center.Y + Min.Y, Maths::Min(point.Y, Center.Y + Max.Y)),
		Maths::Max(Center.Z + Min.Z, Maths::Min(point.Z, Center.Z + Max.Z))
	};

	return boxPoint;
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
	Bounds bounds = _bounds;
	if (auto optionalMesh = GameObject.lock()->TryGetComponent<MeshComponent>(); optionalMesh.has_value() && _useMeshBounds)
		bounds = optionalMesh.value().lock()->GetBounds();

	float scale = Vector3(GameObject.lock()->Transform->GetScale()).Magnitude();
	bounds.Min *= scale;
	bounds.Max *= scale;
	bounds.Center = GameObject.lock()->Transform->GetPosition();

	//_bounds = bounds;
	return bounds;
}

bool AABBColliderComponent::Intersects(const Bounds& other)
{
	auto [Centre, Min, Max] = GetBounds();
	bool xCheck = Centre.X + Min.X <= other.Center.X + other.Max.X
	&& Centre.X + Max.X >= other.Center.X + other.Min.X;
	bool yCheck = Centre.Y + Min.Y <= other.Center.Y + other.Max.Y
	&& Centre.Y + Max.Y >= other.Center.Y + other.Min.Y;
	bool zCheck = Centre.Z + Min.Z <= other.Center.Z + other.Max.Z
	&& Centre.Z + Max.Z >= other.Center.Z + other.Min.Z;

	return xCheck && yCheck && zCheck;
}

CollisionResponse AABBColliderComponent::SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

	Vector3 thisPos = GameObject.lock()->Transform->GetPosition();
	Vector3 closestPoint = ClosestPoint(thisPos);

	if (!collider->IsPointInsideSphere(closestPoint))
		return {};

	Vector3 pos = GameObject.lock()->Transform->GetPosition();

	std::weak_ptr<PhysicsComponent> physComp = {};
	if (auto phys = collider->GameObject.lock()->TryGetComponent<PhysicsComponent>();
		phys.has_value())
		physComp = phys.value();

	Vector3 colliderClosestPoint = collider->ClosestPoint(pos);

	CollisionResponse response{};
	response.Collider = collider;
	response.Normal = pos - collider->ClosestPoint(pos);
	response.Transform = collider->GameObject.lock()->Transform;
	response.ClosestPointOnCollider = collider->ClosestPoint(pos);
	response.PhysicsComponent = physComp;
	response.PenetrationDepth = Vector3(closestPoint - colliderClosestPoint).Magnitude();

	return response;
}

CollisionResponse AABBColliderComponent::AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

	if (!Intersects(collider->GetBounds()))
		return {};

	Vector3 pos = GameObject.lock()->Transform->GetPosition();

	std::weak_ptr<PhysicsComponent> physComp = {};
	if (auto phys = collider->GameObject.lock()->TryGetComponent<PhysicsComponent>();
		phys.has_value())
		physComp = phys.value();
	
	Vector3 closestPoint = ClosestPoint(collider->GameObject.lock()->Transform->GetPosition());
	Vector3 colliderClosestPoint = collider->ClosestPoint(pos);

	CollisionResponse response{};
	response.Collider = collider;
	response.Normal = pos - collider->ClosestPoint(pos);
	response.Transform = collider->GameObject.lock()->Transform;
	response.ClosestPointOnCollider = collider->ClosestPoint(pos);
	response.PhysicsComponent = physComp;
	response.PenetrationDepth = Vector3(closestPoint - colliderClosestPoint).Magnitude();

	return response;
}

CollisionResponse AABBColliderComponent::PlaneCollideCheck(std::shared_ptr<PlaneColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

	Vector3 aabbPosition = GameObject.lock()->Transform->GetPosition();
	Vector3 positiveExtent = GetBounds().Max - aabbPosition;

	//equation from: https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
	//float r = e[0]*Abs(p.n[0]) + e[1]*Abs(p.n[1]) + e[2]*Abs(p.n[2]);
	Vector3 normal = collider->GetNormal();

	float radius = positiveExtent.X * std::abs(normal.X) + positiveExtent.Y * std::abs(normal.Y) + positiveExtent.Z *
		std::abs(normal.Z);

	float distance = normal.Dot(aabbPosition) - collider->GetDistance();

	if (!(std::abs(distance) <= radius))
		return {};

	Vector3 pos = GameObject.lock()->Transform->GetPosition();

	std::weak_ptr<PhysicsComponent> physComp = {};
	if (auto phys = collider->GameObject.lock()->TryGetComponent<PhysicsComponent>();
		phys.has_value())
		physComp = phys.value();

	Vector3 closestPoint = ClosestPoint(collider->GameObject.lock()->Transform->GetPosition());
	Vector3 colliderClosestPoint = collider->ClosestPoint(pos);

	CollisionResponse response{};
	response.Collider = collider;
	response.Normal = pos - collider->ClosestPoint(pos);
	response.Transform = collider->GameObject.lock()->Transform;
	response.ClosestPointOnCollider = collider->ClosestPoint(pos);
	response.PhysicsComponent = physComp;
	response.PenetrationDepth = Vector3(closestPoint - colliderClosestPoint).Magnitude();

	return response;
}
