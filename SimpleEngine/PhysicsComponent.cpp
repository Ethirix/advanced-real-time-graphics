#include "PhysicsComponent.h"

#include "GameObject.h"
#include "Vector3.h"

PhysicsComponent::PhysicsComponent(WP_GAMEOBJECT owningGameObject, float mass) : ComponentBase(owningGameObject)
{
	_mass = mass;
}

void PhysicsComponent::FixedUpdate(double fixedDeltaTime)
{
	auto transform = GameObject.lock()->Transform;
	Vector3 deltaPosition = transform->GetPosition();

	_acceleration += _netForce / _mass;

	_velocity.X += _acceleration.X * fixedDeltaTime;
	_velocity.Y += _acceleration.Y * fixedDeltaTime;
	_velocity.Z += _acceleration.Z * fixedDeltaTime;

	deltaPosition.X += _velocity.X * fixedDeltaTime;
	deltaPosition.Y += _velocity.Y * fixedDeltaTime;
	deltaPosition.Z += _velocity.Z * fixedDeltaTime;

	_netForce = Vector3::Zero;
	_acceleration = Vector3::Zero;

	transform->SetPosition(deltaPosition.ToDXFloat3());
}

Vector3 PhysicsComponent::GetVelocity()
{
	return _velocity;
}

void PhysicsComponent::AddForce(Vector3 force)
{
	_netForce += force;
}

void PhysicsComponent::AddForce(float x, float y, float z)
{
	_netForce.X += x;
	_netForce.Y += y;
	_netForce.Z += z;
}

Vector3 PhysicsComponent::GetNetForce()
{
	return _netForce;
}