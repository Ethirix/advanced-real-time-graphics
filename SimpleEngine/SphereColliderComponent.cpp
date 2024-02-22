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

}

//CollisionResponse response{};
//response.Collider = collider;
//response.Normal = closestPoint - collider->ClosestPoint(pos);
//response.Transform = collider->GameObject.lock()->Transform;
//response.ClosestPointOnCollider = collider->ClosestPoint(pos);
//response.PhysicsComponent = physComp;
//response.PenetrationDepth = Vector3(closestPoint - colliderClosestPoint).Magnitude();

//return response;

CollisionResponse SphereColliderComponent::SphereCollideCheck(std::shared_ptr<SphereColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

}

CollisionResponse SphereColliderComponent::AABBCollideCheck(std::shared_ptr<AABBColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

}

CollisionResponse SphereColliderComponent::PlaneCollideCheck(std::shared_ptr<PlaneColliderComponent> collider)
{
	if (!Collideable || !collider->Collideable)
		return {};

}
