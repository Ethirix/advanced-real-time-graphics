#pragma once
#include <DirectXMath.h>
#include <nlohmann/json.hpp>

#include "ComponentBase.h"
#include "Screen.h"

class CameraComponent :
    public ComponentBase
{
public:
	explicit CameraComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json);

	void Update(double deltaTime) override;
	void FixedUpdate(double fixedDeltaTime) override {}
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {}

	[[nodiscard]] float GetFieldOfView() const { return _fieldOfView; }

	[[nodiscard]] DirectX::XMFLOAT4X4& GetView() { return _view; }
	[[nodiscard]] DirectX::XMFLOAT4X4& GetProjection() { return _projection; }
	[[nodiscard]] float GetNearDepth() { return _nearDepth; }
	[[nodiscard]] float GetFarDepth() { return _farDepth; }

private:
	[[nodiscard]] DirectX::XMFLOAT3 GetForward(DirectX::XMFLOAT3 rot);
	[[nodiscard]] DirectX::XMFLOAT3 GetRight(DirectX::XMFLOAT3 rot);
	[[nodiscard]] DirectX::XMFLOAT3 GetUp(DirectX::XMFLOAT3 rot);

	DirectX::XMFLOAT4X4 _view, _projection;

	float _nearDepth, _farDepth, _fieldOfView;
	float _movementSpeed, _rotationSpeed;
	std::shared_ptr<Screen> _screen;
};