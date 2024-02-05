#include "PhysicsComponent.h"

#include "Constants.h"
#include "GameObject.h"
#include "Vector3.h"

PhysicsComponent::PhysicsComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json)
	: ComponentBase(owningGameObject)
{
	_mass = json["Mass"];

	DragCoefficient = json["DragCoefficient"];
	Gravity = json["Gravity"];
}

void PhysicsComponent::CalculateNetForce()
{
	if (Gravity)
		_netForce += GRAVITY;

	Vector3 vel = _velocity;
	vel = { -vel.X, -vel.Y,-vel.Z };
	vel = vel.Normalise();
	vel *= Drag();

	_netForce += vel;
}

float PhysicsComponent::Drag()
{
	return 0.5f * AIR_DENSITY * (_velocity.Magnitude() * _velocity.Magnitude()) * DragCoefficient * 1;
}

void PhysicsComponent::FixedUpdate(double fixedDeltaTime)
{
	auto transform = GameObject.lock()->Transform;
	Vector3 deltaPosition = transform->GetPosition();

	CalculateNetForce();

	_acceleration += _netForce / _mass;

	_velocity.X += _acceleration.X * fixedDeltaTime;
	_velocity.Y += _acceleration.Y * fixedDeltaTime;
	_velocity.Z += _acceleration.Z * fixedDeltaTime;

	deltaPosition.X += _velocity.X;
	deltaPosition.Y += _velocity.Y;
	deltaPosition.Z += _velocity.Z;

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