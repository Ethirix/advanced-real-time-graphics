#include "ColliderComponent.h"

#include "GameObject.h"
#include "SphereColliderComponent.h"

ColliderComponent::ColliderComponent(WP_GAMEOBJECT owningGameObject)
	: ComponentBase(owningGameObject)
{
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
		}
		break;
	default:
		break;
	}

	return false;
}
