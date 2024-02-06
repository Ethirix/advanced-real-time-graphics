#include "PhysicsComponent.h"

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

	if (UseFriction)
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
	DirectX::XMFLOAT4 dxQuaternion = GameObject.lock()->Transform->GetRotation();
	Quaternion quaternion = {dxQuaternion.w, dxQuaternion.x, dxQuaternion.y, dxQuaternion.z };
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