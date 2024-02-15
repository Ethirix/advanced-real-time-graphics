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
	float GetMass();

	void SetMass(float mass);

	void AddForce(Vector3 force);
	void AddForce(float x, float y, float z);
	void ApplyImpulse(Vector3 force);
	void ApplyImpulse(float x, float y, float z);

	bool AddRelativeForce(Vector3 force, Vector3 point);

	void RunCollisionImpulse(CollisionResponse response);
private:
	void CalculateNetForce();

	Vector3 CalculateDrag();
	Vector3 CalculateFriction();

	float CalculateNormalForce();

public:
	bool UseGravity = true;
	bool UseDrag = true;
	bool UseFriction = true;

	float DragCoefficient = 1;
	float FrictionCoefficient = 0.5f;
	float Restitution = 0.0f;

private:
	Vector3 _velocity{};
	Vector3 _acceleration{};
	Vector3 _netForce{};

	Vector3 _torque{};

	float _mass = 1.0f;

	float _crossSectionalArea = 1;
};
