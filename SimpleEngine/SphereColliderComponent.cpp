#include "SphereColliderComponent.h"

#include "AABBColliderComponent.h";
#include "GameObject.h"
#include "Maths.h"
#include "PlaneColliderComponent.h"
#include "SceneGraph.h"
#include <cmath>

#include "Debug.h"

SphereColliderComponent::SphereColliderComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json)
	: ColliderComponent(owningGameObject)
{
	_radius = json["Radius"];
	Collideable = json["Collideable"];
	Type = COLLIDER_SPHERE;
}

float SphereColliderComponent::GetRadius()
{
	return _radius;
}

DirectX::XMFLOAT3X3 SphereColliderComponent::GetInertiaTensor(float mass)
{
	DirectX::XMFLOAT3X3 tensor;
	tensor.m[0][0] = 2 / 5.0f * mass * Maths::Pow(_radius, 2);
	tensor.m[1][1] = 2 / 5.0f * mass * Maths::Pow(_radius, 2);
	tensor.m[2][2] = 2 / 5.0f * mass * Maths::Pow(_radius, 2);

	return tensor;
}

Vector3 SphereColliderComponent::ClosestPoint(Vector3 point)
{
	Vector3 pos = GameObject.lock()->Transform->GetPosition();
	Vector3 difference = Vector3::Normalise(point - pos);
	difference *= _radius;

	return pos + difference;
}

CollisionResponse SphereColliderComponent::SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

	Vector3 pos = GameObject.lock()->Transform->GetPosition();
	Vector3 otherPos = collider->GameObject.lock()->Transform->GetPosition();

	if (Vector3::Magnitude(pos - otherPos) > _radius + collider->GetRadius())
		return {};

	Vector3 closestPoint = ClosestPoint(otherPos);
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

CollisionResponse SphereColliderComponent::AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

	Vector3 pos = GameObject.lock()->Transform->GetPosition();
	Vector3 closestPoint = ClosestPoint(collider->GameObject.lock()->Transform->GetPosition());
	Vector3 colliderClosestPoint = collider->ClosestPoint(pos);

	Vector3 difference = closestPoint - colliderClosestPoint;
	float magSqr = difference.MagnitudeSqr();

	if (magSqr >= Maths::Pow(_radius, 2))
		return {};

	CollisionResponse response{};
	response.Collider = collider;
	response.Normal = Vector3::Normalise(pos - colliderClosestPoint);
	response.Transform = collider->GameObject.lock()->Transform;
	response.ClosestPointOnCollider = colliderClosestPoint;
	response.PhysicsComponent = collider->GameObject.lock()->GetComponent<PhysicsComponent>();
	response.PenetrationDepth = Maths::Pow(_radius, 2) - magSqr;

	return response;
}

CollisionResponse SphereColliderComponent::PlaneCollideCheck(std::shared_ptr<PlaneColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};
		
	Vector3 pos = GameObject.lock()->Transform->GetPosition();
	Vector3 colliderClosestPoint = collider->ClosestPoint(pos);

	//https://zhangmenghe.github.io/cse291d-PhysicsSimulation.html
	Vector3 offset = pos - colliderClosestPoint;
	if (std::abs(offset.Dot(collider->GetNormal())) > _radius)
		return {};

	CollisionResponse response{};
	response.Collider = collider;
	response.Normal = Vector3::Normalise(pos - colliderClosestPoint);
	response.Transform = collider->GameObject.lock()->Transform;
	response.ClosestPointOnCollider = colliderClosestPoint;
	response.PhysicsComponent = collider->GameObject.lock()->GetComponent<PhysicsComponent>();
	response.PenetrationDepth = std::abs(Vector3(colliderClosestPoint - pos).Magnitude() - _radius);

	return response;
}
