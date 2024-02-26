#include "PlaneColliderComponent.h"

#include "AABBColliderComponent.h"
#include "Maths.h"
#include "PhysicsComponent.h"
#include "SceneGraph.h"
#include "SphereColliderComponent.h"

PlaneColliderComponent::PlaneColliderComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json)
	: ColliderComponent(owningGameObject)
{
	Type = COLLIDER_PLANE;

	nlohmann::json v3 = json["Normal"];
	_distance = json["Distance"];
	_normal = Vector3::Normalise({v3["X"], v3["Y"], v3["Z"]});

	Collideable = json["Collideable"];
}

void PlaneColliderComponent::SetNormal(Vector3 normal)
{
	_normal = normal.Normalise();
}

void PlaneColliderComponent::SetDistance(float distance)
{
	_distance = distance;
}

float PlaneColliderComponent::GetDistance()
{
	return _distance;
}

Vector3 PlaneColliderComponent::GetNormal()
{
	_normal = _normal.Normalise();
	return _normal;
}

DirectX::XMFLOAT3X3 PlaneColliderComponent::GetInertiaTensor(float mass)
{
	return {};
}

bool PlaneColliderComponent::Intersects(const std::shared_ptr<PlaneColliderComponent>& plane)
{
	Vector3 direction = GetNormal().Cross(plane->GetNormal());
	return direction.Dot(direction) > FLT_EPSILON;
}

bool PlaneColliderComponent::PointOnPlane(Vector3 point)
{
	return point.Dot(GetNormal()) - _distance == 0;
}

Vector3 PlaneColliderComponent::ClosestPoint(Vector3 point)
{
	Vector3 normal = GetNormal();
	float distance = normal.Dot(point) - _distance;

	return point - normal * distance;
}

CollisionResponse PlaneColliderComponent::PlaneCollideCheck(std::shared_ptr<PlaneColliderComponent> collider)
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
	response.Normal = Vector3::Normalise(closestPoint - colliderClosestPoint);
	response.Transform = collider->GameObject.lock()->Transform;
	response.ClosestPointOnCollider = colliderClosestPoint;
	response.PhysicsComponent = collider->GameObject.lock()->GetComponent<PhysicsComponent>();
	response.PenetrationDepth = Vector3(closestPoint - colliderClosestPoint).Magnitude();

	return response;
}

CollisionResponse PlaneColliderComponent::AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

	Bounds bounds = collider->GetBounds();
	Vector3 planeNormal = GetNormal();

	//https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
	float projectionRadius = bounds.Extents.X * std::abs(planeNormal.X) + bounds.Extents.Y * std::abs(planeNormal.Y)
		+ bounds.Extents.Z * std::abs(planeNormal.Z);

	float distance = planeNormal.Dot(bounds.Center) - _distance;

	if (std::abs(distance) > projectionRadius)
		return {};

	Vector3 pos = GameObject.lock()->Transform->GetPosition();
	Vector3 closestPoint = ClosestPoint(collider->GameObject.lock()->Transform->GetPosition());
	Vector3 colliderClosestPoint = collider->ClosestPoint(pos);

	CollisionResponse response{};
	response.Collider = collider;
	response.Normal = Vector3::Normalise(closestPoint - colliderClosestPoint);
	response.Transform = collider->GameObject.lock()->Transform;
	response.ClosestPointOnCollider = colliderClosestPoint;
	response.PhysicsComponent = collider->GameObject.lock()->GetComponent<PhysicsComponent>();
	response.PenetrationDepth = Vector3(closestPoint - colliderClosestPoint).Magnitude();

	return response;
}

CollisionResponse PlaneColliderComponent::SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};
		
	Vector3 otherPos = collider->GameObject.lock()->Transform->GetPosition();
	Vector3 closestPoint = ClosestPoint(otherPos);

	//https://zhangmenghe.github.io/cse291d-PhysicsSimulation.html
	Vector3 offset = otherPos - closestPoint;
	if (std::abs(offset.Dot(GetNormal())) > collider->GetRadius())
		return {};

	Vector3 pos = GameObject.lock()->Transform->GetPosition();
	Vector3 colliderClosestPoint = collider->ClosestPoint(pos);

	CollisionResponse response{};
	response.Collider = collider;
	response.Normal = Vector3::Normalise(pos - colliderClosestPoint);
	response.Transform = collider->GameObject.lock()->Transform;
	response.ClosestPointOnCollider = colliderClosestPoint;
	response.PhysicsComponent = collider->GameObject.lock()->GetComponent<PhysicsComponent>();
	response.PenetrationDepth = Vector3(closestPoint - colliderClosestPoint).Magnitude();

	return response;
}