﻿#include "PhysicsComponent.h"

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
	AngularDamping = json["AngularDamping"];
}

void PhysicsComponent::CalculateNetForce(double fixedDeltaTime)
{
	if (UseDrag)
		_netForce += CalculateDrag();

	CalculateAngularVelocity(fixedDeltaTime);
}

Vector3 PhysicsComponent::CalculateFriction(CollisionResponse response)
{
	Vector3 velocity = _velocity;
	velocity = Vector3::Zero() - velocity;
	velocity = velocity.Normalise();
	return (velocity + response.Normal) * FrictionCoefficient;
}

Vector3 PhysicsComponent::CalculateDrag()
{
	Vector3 velocity = _velocity;
	velocity = Vector3::Zero() - velocity;
	velocity = velocity.Normalise();	
	return velocity * (0.5f * AIR_DENSITY * _velocity.MagnitudeSqr() * DragCoefficient * _crossSectionalArea);
}

void PhysicsComponent::RunCollisionImpulse(CollisionResponse response)
{
	if (_mass == 0)
		return;

	Vector3 rv = response.PhysicsComponent.expired()
		? _velocity
		: _velocity - response.PhysicsComponent.lock()->GetVelocity();
	
	float vj = -(1 + Restitution) * rv.Dot(response.Normal);

	float j = response.PhysicsComponent.expired() || response.PhysicsComponent.lock()->GetMass() == 0
		? vj / (1 / _mass)
		: vj / (1 / _mass + 1 / response.PhysicsComponent.lock()->GetMass());

	_impulse += response.Normal * (1 / _mass * j);

	if (UseFriction)
		_netForce += CalculateFriction(response);

	_positionOffset += response.Normal * response.PenetrationDepth;
}

void PhysicsComponent::CalculateAngularVelocity(double fixedDeltaTime)
{
	auto optionalCollider = GameObject.lock()->TryGetComponent<ColliderComponent>();
	if (!optionalCollider.has_value())
		return;

	DirectX::XMFLOAT3X3 inertiaTensor = optionalCollider.value().lock()->GetInertiaTensor(_mass);
	DirectX::XMMATRIX matrix = XMMatrixInverse(nullptr, XMLoadFloat3x3(&inertiaTensor));
	DirectX::XMFLOAT3 torqueXM3 = Vector3::ToDXFloat3(_torque * fixedDeltaTime);
	DirectX::XMVECTOR torqueVector = XMLoadFloat3(&torqueXM3);
	DirectX::XMVECTOR angularAccelerationXMV3 = XMVector3Transform(torqueVector, matrix);
	DirectX::XMFLOAT3 angularAcceleration{};
	XMStoreFloat3(&angularAcceleration, angularAccelerationXMV3);

	_angularVelocity += Vector3(angularAcceleration);
	Quaternion q = GameObject.lock()->Transform->GetRotation();
	q += q * _angularVelocity * (1 / 2.0f);
	if (q.Magnitude() != 0)
		q /= q.Magnitude();
	GameObject.lock()->Transform->SetRotation(q);
}

void PhysicsComponent::AddRelativeForce(Vector3 force, Vector3 point)
{
	AddForce(force);
	_torque = point.Cross(force);
}

void PhysicsComponent::FixedUpdate(double fixedDeltaTime)
{
	if (_mass == 0)
		return;

	auto& transform = GameObject.lock()->Transform;
	transform->AddToPosition(_positionOffset.ToDXFloat3());
	Vector3 deltaPosition = transform->GetPosition();

	CalculateNetForce(fixedDeltaTime);
	_acceleration += _netForce / _mass;

	if (UseGravity)
		_acceleration += GRAVITY * fixedDeltaTime;

	_velocity += _impulse;
	_velocity.X += _acceleration.X * fixedDeltaTime;
	_velocity.Y += _acceleration.Y * fixedDeltaTime;
	_velocity.Z += _acceleration.Z * fixedDeltaTime;

	deltaPosition.X += _velocity.X;
	deltaPosition.Y += _velocity.Y;
	deltaPosition.Z += _velocity.Z;

	_netForce = Vector3::Zero();
	_acceleration = Vector3::Zero();
	_torque = Vector3::Zero();
	_impulse = Vector3::Zero();
	_positionOffset = Vector3::Zero();
	_angularVelocity *= std::powf(AngularDamping, fixedDeltaTime);

	transform->SetPosition(deltaPosition.ToDXFloat3());
}

Vector3 PhysicsComponent::GetVelocity()
{
	return _velocity;
}

Vector3 PhysicsComponent::GetAngularVelocity()
{
	return _angularVelocity;
}

Vector3 PhysicsComponent::GetAcceleration()
{
	return _acceleration;
}

Vector3 PhysicsComponent::GetNetForce()
{
	return _netForce;
}

Vector3 PhysicsComponent::GetTorque()
{
	return _torque;
}

Vector3 PhysicsComponent::GetImpulse()
{
	return _impulse;
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