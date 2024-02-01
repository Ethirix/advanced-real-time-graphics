#include "PhysicsComponent.h"

#include "GameObject.h"

PhysicsComponent::PhysicsComponent(WP_GAMEOBJECT owningGameObject, DirectX::XMFLOAT3 initialVelocity, 
		DirectX::XMFLOAT3 initialAcceleration) : ComponentBase(owningGameObject)
{
	_velocity = initialVelocity;
	_acceleration = initialAcceleration;
}

void PhysicsComponent::FixedUpdate(double fixedDeltaTime)
{
	auto transform = GameObject.lock()->Transform;
	DirectX::XMFLOAT3 deltaPosition = transform->GetPosition();

	_velocity.x += _acceleration.x * fixedDeltaTime;
	_velocity.y += _acceleration.y * fixedDeltaTime;
	_velocity.z += _acceleration.z * fixedDeltaTime;

	deltaPosition.x += _velocity.x * fixedDeltaTime;
	deltaPosition.y += _velocity.y * fixedDeltaTime;
	deltaPosition.z += _velocity.z * fixedDeltaTime;

	transform->SetPosition(deltaPosition);
}

void PhysicsComponent::SetVelocity(DirectX::XMFLOAT3 velocity)
{
	_velocity = velocity;
}

void PhysicsComponent::SetVelocity(float x, float y, float z)
{
	_velocity = {x, y, z};
}

DirectX::XMFLOAT3 PhysicsComponent::GetVelocity()
{
	return _velocity;
}

void PhysicsComponent::SetAcceleration(DirectX::XMFLOAT3 acceleration)
{
	_acceleration = acceleration;
}

void PhysicsComponent::SetAcceleration(float x, float y, float z)
{
	_acceleration = {x, y, z};
}

DirectX::XMFLOAT3 PhysicsComponent::GetAcceleration()
{
	return _acceleration;
}