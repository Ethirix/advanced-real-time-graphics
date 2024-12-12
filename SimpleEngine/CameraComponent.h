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
	[[nodiscard]] DirectX::XMFLOAT3& GetAt() { return _at; }
	[[nodiscard]] DirectX::XMFLOAT3& GetEye() { return _eye; }
	[[nodiscard]] DirectX::XMFLOAT3& GetUp() { return _up; }
	[[nodiscard]] DirectX::XMFLOAT4X4& GetView() { return _view; }
	[[nodiscard]] DirectX::XMFLOAT4X4& GetProjection() { return _projection; }
	[[nodiscard]] float GetNearDepth() { return _nearDepth; }
	[[nodiscard]] float GetFarDepth() { return _farDepth; }

	void SetAt(const DirectX::XMFLOAT3 at) { _at = at; }
	void SetEye(const DirectX::XMFLOAT3 eye) { _eye = eye; }
	void SetUp(const DirectX::XMFLOAT3 up) { _up = up; }

private:
	DirectX::XMFLOAT3 _eye, _at, _up;
	DirectX::XMFLOAT4X4 _view, _projection;

	float _nearDepth, _farDepth, _fieldOfView;
	float _movementSpeed, _rotationSpeed;
	std::shared_ptr<Screen> _screen;
};
