#pragma once
#include <DirectXMath.h>
#include <memory>
#include <vector>

#include "ComponentBase.h"
#include "Quaternion.h"
class TransformComponent final:
    public ComponentBase
{
public:
	std::weak_ptr<TransformComponent> Parent;
	std::vector <std::weak_ptr<TransformComponent>> Children;
	DirectX::XMFLOAT4X4 WorldMatrix;

private:
	Vector3 _localPosition = { 0, 0, 0 };
	Vector3 _scale = { 1, 1, 1 };
	Quaternion _quaternion = {};

public:
	explicit TransformComponent(
		const std::weak_ptr<::GameObject>& owningGameObject, 
		const std::weak_ptr<TransformComponent>& parent = {})
	: ComponentBase(owningGameObject)
	{
		WorldMatrix = {};
		Parent = parent;
	}

    void Update(double deltaTime) override;
    void FixedUpdate(double fixedDeltaTime) override {};
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override {};

	void SetPosition(DirectX::XMFLOAT3 pos);
	void SetPosition(float x, float y, float z);
	void AddToPosition(DirectX::XMFLOAT3 pos);
	void AddToPosition(float x, float y, float z);

	void SetScale(DirectX::XMFLOAT3 scale);
	void SetScale(float x, float y, float z);

	void SetRotation(DirectX::XMFLOAT3 rot);
	void SetRotation(float x, float y, float z);
	void SetRotation(Quaternion q);
	void AddToRotation(DirectX::XMFLOAT3 rot);
	void AddToRotation(float x, float y, float z);

	[[nodiscard]] DirectX::XMFLOAT3 GetPosition();
	[[nodiscard]] DirectX::XMFLOAT3 GetScale(bool recursiveBlock = false);

	[[nodiscard]] DirectX::XMFLOAT4 GetRotation();
	[[nodiscard]] DirectX::XMFLOAT3 GetWorldPosition();
};