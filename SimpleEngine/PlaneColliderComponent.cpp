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

void PlaneColliderComponent::FixedUpdate(double fixedDeltaTime)
{
	std::list<CollisionResponse> collision = SceneGraph::CheckColliders(GetColliderPtr());
	//do collider stuff ig

	if (auto optionalPhys = GameObject.lock()->TryGetComponent<PhysicsComponent>();
		optionalPhys.has_value() && !collision.empty())
		optionalPhys.value().lock()->AddForce(0, 25, 0);
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
	return _normal;
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

Vector3 PlaneColliderComponent::ClosestPointOnPlane(Vector3 point)
{
	Vector3 normal = GetNormal();
	float distance = normal.Dot(point) - _distance;

	return point - normal * distance;
}

bool PlaneColliderComponent::AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return false;

	Vector3 aabbPosition = collider->GameObject.lock()->Transform->GetPosition();
	Vector3 positiveExtent = collider->GetBounds().Max - aabbPosition;

	//equation from: https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
	//float r = e[0]*Abs(p.n[0]) + e[1]*Abs(p.n[1]) + e[2]*Abs(p.n[2]);
	Vector3 normal = GetNormal();

	float radius = positiveExtent.X * std::abs(normal.X) + positiveExtent.Y * std::abs(normal.Y) + positiveExtent.Z *
		std::abs(normal.Z);


	float distance = normal.Dot(aabbPosition) - _distance;

	return std::abs(distance) <= radius;
}

bool PlaneColliderComponent::PlaneCollideCheck(std::shared_ptr<PlaneColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return false;

	return Intersects(collider);
}

bool PlaneColliderComponent::SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return false;

	Vector3 closestPoint = ClosestPointOnPlane(collider->GameObject.lock()->Transform->GetPosition());

	return closestPoint.MagnitudeSqr() <= Maths::Pow(collider->GetRadius(), 2);
}