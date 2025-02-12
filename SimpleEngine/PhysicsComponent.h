#pragma once
#include <nlohmann/json.hpp>

#include "ComponentBase.h"
#include "Vector3.h"

class GameObject;
class CollisionResponse;
class PhysicsComponent : public ComponentBase
{
public:
	explicit PhysicsComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json);

	void Update(double deltaTime) override {}
	void FixedUpdate(double fixedDeltaTime) override;
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}

	Vector3 GetVelocity();
	Vector3 GetAngularVelocity();
	Vector3 GetAcceleration();
	Vector3 GetNetForce();
	Vector3 GetTorque();
	Vector3 GetImpulse();
	float GetMass();

	void SetMass(float mass);

	void AddForce(Vector3 force);
	void AddForce(float x, float y, float z);

	void AddRelativeForce(Vector3 force, Vector3 point);

	void RunCollisionImpulse(CollisionResponse response);
private:
	void CalculateNetForce(double fixedDeltaTime);

	Vector3 CalculateDrag();
	Vector3 CalculateFriction(CollisionResponse response);
	void CalculateAngularVelocity(double fixedDeltaTime);

public:
	bool UseGravity = true;
	bool UseDrag = true;
	bool UseFriction = true;

	float DragCoefficient = 1;
	float FrictionCoefficient = 0.5f;
	float Restitution = 0.0f;
	float AngularDamping = 0.99f;

private:
	Vector3 _velocity{};
	Vector3 _angularVelocity{};
	Vector3 _acceleration{};
	Vector3 _netForce{};
	Vector3 _positionOffset{};
	Vector3 _torque{};
	Vector3 _impulse{};

	float _mass = 1.0f;

	float _crossSectionalArea = 1;
};
