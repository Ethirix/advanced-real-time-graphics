#include "SphereColliderComponent.h"

#include "AABBColliderComponent.h";
#include "GameObject.h"
#include "Maths.h"
#include "PlaneColliderComponent.h"
#include "SceneGraph.h"

SphereColliderComponent::SphereColliderComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json)
	: ColliderComponent(owningGameObject)
{
	_radius = json["Radius"];
	Collideable = json["Collideable"];
	Type = COLLIDER_SPHERE;
}

Vector3 SphereColliderComponent::ClosestPoint(Vector3 point)
{
	Vector3 spherePos = GameObject.lock()->Transform->GetPosition();
	Vector3 sphereToPoint = point - spherePos;
	sphereToPoint = sphereToPoint.Normalise();

	sphereToPoint *= _radius;

	return spherePos + sphereToPoint;
}

float SphereColliderComponent::GetRadius()
{
	return _radius;
}

bool SphereColliderComponent::IsPointInsideSphere(Vector3 point)
{
	Vector3 thisPos = GameObject.lock()->Transform->GetPosition();
	float distance = Vector3(point - thisPos).MagnitudeSqr();

	return distance < _radius * _radius;
}

bool SphereColliderComponent::SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return false;

	Vector3 thisPos = GameObject.lock()->Transform->GetPosition();
	Vector3 collPos = collider->GameObject.lock()->Transform->GetPosition();

	if (Vector3(collPos - thisPos).Magnitude() < _radius + collider->GetRadius())
		return true;

	return false;
}

bool SphereColliderComponent::AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return false;

	Vector3 thisPos = GameObject.lock()->Transform->GetPosition();
	Vector3 closestPoint = collider->ClosestPoint(thisPos);

	return IsPointInsideSphere(closestPoint);
}

bool SphereColliderComponent::PlaneCollideCheck(std::shared_ptr<PlaneColliderComponent> collider)
{
	Vector3 closestPoint = collider->ClosestPoint(GameObject.lock()->Transform->GetPosition());

	return closestPoint.MagnitudeSqr() <= Maths::Pow(_radius, 2);
}
