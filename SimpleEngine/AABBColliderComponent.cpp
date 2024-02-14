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

void AABBColliderComponent::FixedUpdate(double fixedDeltaTime)
{
	std::list<CollisionResponse> collision = SceneGraph::CheckColliders(GetColliderPtr());
	//do collider stuff ig

	if (auto optionalPhys = GameObject.lock()->TryGetComponent<PhysicsComponent>();
		optionalPhys.has_value() && !collision.empty())
		optionalPhys.value().lock()->AddForce(0, 25, 0);
}

Vector3 AABBColliderComponent::GetClosestPoint(Vector3 point)
{
	auto [Center, Min, Max] = GetBounds();
	Vector3 boxPoint = {
		Maths::Max(Center.X + Min.X, Maths::Min(point.X, Center.X + Max.X)),
		Maths::Max(Center.Y + Min.Y, Maths::Min(point.Y, Center.Y + Max.Y)),
		Maths::Max(Center.Z + Min.Z, Maths::Min(point.Z, Center.Z + Max.Z))
	};

	return boxPoint;
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

	_bounds = bounds;
	return bounds;
}

bool AABBColliderComponent::Intersects(const Bounds& other)
{
	auto [Center, Min, Max] = GetBounds();
	bool xCheck = Max.X <= other.Min.X && Max.X >= Min.X;
	bool yCheck = Max.Y <= other.Min.Y && Max.Y >= Min.Y;
	bool zCheck = Max.Z <= other.Min.Z && Max.Z >= Min.Z;

	return xCheck && yCheck && zCheck;
}

bool AABBColliderComponent::SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return false;

	Vector3 thisPos = GameObject.lock()->Transform->GetPosition();
	Vector3 closestPoint = GetClosestPoint(thisPos);

	return collider->IsPointInsideSphere(closestPoint);
}

bool AABBColliderComponent::AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return false;

	return Intersects(collider->GetBounds());
}

bool AABBColliderComponent::PlaneCollideCheck(std::shared_ptr<PlaneColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return false;

	Vector3 aabbPosition = GameObject.lock()->Transform->GetPosition();
	Vector3 positiveExtent = GetBounds().Max - aabbPosition;

	//equation from: https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
	//float r = e[0]*Abs(p.n[0]) + e[1]*Abs(p.n[1]) + e[2]*Abs(p.n[2]);
	Vector3 normal = collider->GetNormal();

	float radius = positiveExtent.X * std::abs(normal.X) + positiveExtent.Y * std::abs(normal.Y) + positiveExtent.Z *
		std::abs(normal.Z);


	float distance = normal.Dot(aabbPosition) - collider->GetDistance();

	return std::abs(distance) <= radius;
}
