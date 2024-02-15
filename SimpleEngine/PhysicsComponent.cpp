#include "PhysicsComponent.h"

#include "CollisionResponse.h"
#include "Constants.h"
#include "GameObject.h"
#include "Quaternion.h"
#include "Vector3.h"

PhysicsComponent::PhysicsComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json)
	: ComponentBase(owningGameObject)
{
	_mass = json["Mass"];

	DragCoefficient = json["DragCoefficient"];
	FrictionCoefficient = json["FrictionCoefficient"];
	Restitution = json["Restitution"];

	UseGravity = json["UseGravity"];
	UseDrag = json["UseDrag"];
	UseFriction = json["UseFriction"];
}

void PhysicsComponent::CalculateNetForce()
{
	if (UseGravity)
		_netForce += GRAVITY;

	if (UseDrag)
		_netForce += CalculateDrag();

	if (UseFriction) //disabled until collision detection
		_netForce += CalculateFriction();
}

Vector3 PhysicsComponent::CalculateFriction()
{
	Vector3 velocity = _velocity;
	velocity = Vector3::Zero() - velocity;
	velocity = velocity.Normalise();
	return velocity * (FrictionCoefficient * CalculateNormalForce());
}

float PhysicsComponent::CalculateNormalForce()
{
	Quaternion quaternion = GameObject.lock()->Transform->GetRotation();
	Vector3 euler = MakeEulerAnglesFromQ(quaternion);
	return GRAVITY.Magnitude() * _mass * std::cos(euler.Angle(Vector3::Up()));
}

Vector3 PhysicsComponent::CalculateDrag()
{
	Vector3 velocity = _velocity;
	velocity = Vector3::Zero() - velocity;
	velocity = velocity.Normalise();
	return velocity * (0.5f * AIR_DENSITY * (_velocity.Magnitude() * _velocity.Magnitude()) * DragCoefficient * 1);
}

void PhysicsComponent::RunCollisionImpulse(CollisionResponse response)
{
	Vector3 rv = response.PhysicsComponent.expired()
		? _velocity
		: _velocity - response.PhysicsComponent.lock()->GetVelocity();
	
	float vj = -(1 + Restitution) * rv.Dot(response.Normal);

	float j = response.PhysicsComponent.expired()
		? vj / (1 / _mass + 1)
		: vj / (1 / _mass + 1 / response.PhysicsComponent.lock()->GetMass());

	Vector3 impulse = response.Normal * 1 / _mass * j;

	ApplyImpulse(impulse);
}

bool PhysicsComponent::AddRelativeForce(Vector3 force, Vector3 point)
{
	AddForce(force);
	_torque = point.Cross(force);
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

	_netForce = Vector3::Zero();
	_acceleration = Vector3::Zero();

	transform->SetPosition(deltaPosition.ToDXFloat3());
}

Vector3 PhysicsComponent::GetVelocity()
{
	return _velocity;
}

float PhysicsComponent::GetMass()
{
	return _mass;
}

void PhysicsComponent::SetMass(float mass)
{
	_mass = mass;
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

void PhysicsComponent::ApplyImpulse(Vector3 force)
{
	_velocity += force;
}

void PhysicsComponent::ApplyImpulse(float x, float y, float z)
{
	_velocity += Vector3(x, y, z);
}