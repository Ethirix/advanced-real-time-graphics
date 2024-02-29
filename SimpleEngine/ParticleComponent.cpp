#include "ParticleComponent.h"

ParticleComponent::ParticleComponent(WP_GAMEOBJECT owningGameObject)
	: PhysicsComponent(owningGameObject, nlohmann::json{})
{
	
}
