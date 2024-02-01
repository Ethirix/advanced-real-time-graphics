#pragma once
#include <DirectXMath.h>

#include "ComponentBase.h"

class PhysicsComponent final : public ComponentBase
{
public:
	PhysicsComponent(WP_GAMEOBJECT owningGameObject, DirectX::XMFLOAT3 initialVelocity, 
		DirectX::XMFLOAT3 initialAcceleration);

	void Update(double deltaTime) override {}
	void FixedUpdate(double fixedDeltaTime) override;
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}

	void SetVelocity(DirectX::XMFLOAT3 velocity);
	void SetVelocity(float x, float y, float z);

	DirectX::XMFLOAT3 GetVelocity();

	void SetAcceleration(DirectX::XMFLOAT3 acceleration);
	void SetAcceleration(float x, float y, float z);

	DirectX::XMFLOAT3 GetAcceleration();
private:
private:
	DirectX::XMFLOAT3 _velocity{};
	DirectX::XMFLOAT3 _acceleration{};
};
