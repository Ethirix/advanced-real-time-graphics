#pragma once
#include <nlohmann/json.hpp>

#include "ComponentBase.h"
#include "Vector3.h"

class GameObject;
class PhysicsComponent : public ComponentBase
{
public:
	explicit PhysicsComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json);

	void Update(double deltaTime) override {}
	void FixedUpdate(double fixedDeltaTime) override;
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}

	Vector3 GetVelocity();

	void AddForce(Vector3 force);
	void AddForce(float x, float y, float z);

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

private:
	Vector3 _velocity{};
	Vector3 _acceleration{};
	Vector3 _netForce{};

	float _mass = 1.0f;

	float _crossSectionalArea = 1;
};
