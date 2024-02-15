#include "ColliderComponent.h"

#include "GameObject.h"
#include "SphereColliderComponent.h"
#include "AABBColliderComponent.h"
#include "CollisionResponse.h"
#include "PlaneColliderComponent.h"
#include "SceneGraph.h"

ColliderComponent::ColliderComponent(WP_GAMEOBJECT owningGameObject)
	: ComponentBase(owningGameObject)
{
}

void ColliderComponent::FixedUpdate(double fixedDeltaTime)
{
	RunCollisionResponse(fixedDeltaTime);
}

void ColliderComponent::RunCollisionResponse(double fixedDeltaTime)
{
	std::list<CollisionResponse> collision = SceneGraph::CheckColliders(GetColliderPtr());
	//do collider stuff ig

	for (auto collisionResponse : collision)
	{
		auto optionalPhysComponent = GameObject.lock()->TryGetComponent<PhysicsComponent>();
		if (optionalPhysComponent.has_value())
		{
			optionalPhysComponent.value().lock()->RunCollisionImpulse(collisionResponse);
		}
	}
}

bool ColliderComponent::CollidesWith(const std::shared_ptr<ColliderComponent>& collider)
{
	switch (collider->Type)
	{
	case COLLIDER_SPHERE:
		{
			if (std::shared_ptr<SphereColliderComponent> sphere =
				std::dynamic_pointer_cast<SphereColliderComponent>(collider))
				return SphereCollideCheck(sphere);
			break;
		}
	case COLLIDER_AABB:
		{
			if (std::shared_ptr<AABBColliderComponent> aabb = 
				std::dynamic_pointer_cast<AABBColliderComponent>(collider))
				return AABBCollideCheck(aabb);
			break;
		}
	case COLLIDER_PLANE:
		{
			if (std::shared_ptr<PlaneColliderComponent> plane = 
				std::dynamic_pointer_cast<PlaneColliderComponent>(collider))
				return PlaneCollideCheck(plane);
			break;
		}
	default:
		break;
	}

	return false;
}
