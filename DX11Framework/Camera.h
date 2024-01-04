#pragma once
#include <DirectXMath.h>
#include <memory>

#include "GameObject.h"
#include "Screen.h"

class Camera : public GameObject
{
	friend class GameObjectFactory;
protected:
	Camera() = default;
	DirectX::XMFLOAT3 _eye, _at, _up;
	DirectX::XMFLOAT4X4 _view, _projection;

	float _nearDepth, _farDepth, _fieldOfView;
	std::shared_ptr<Screen> _screen;

public:
	void Update(double deltaTime) override;

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
};