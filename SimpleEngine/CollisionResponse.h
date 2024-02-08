#pragma once
#include "ColliderComponent.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"

struct CollisionResponse
{
	std::weak_ptr<ColliderComponent> Collider;
	std::weak_ptr<TransformComponent> Transform;
	std::weak_ptr<PhysicsComponent> PhysicsComponent;
};
