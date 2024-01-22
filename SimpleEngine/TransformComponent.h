#pragma once
#include <DirectXMath.h>
#include <memory>
#include <vector>
class GameObject;

#include "ComponentBase.h"
class TransformComponent final:
    public ComponentBase
{
public:
	std::weak_ptr<TransformComponent> Parent;
	std::vector <std::weak_ptr<TransformComponent>> Children;
	DirectX::XMFLOAT4X4 WorldMatrix;

private:
	DirectX::XMFLOAT3 _localPosition = { 0, 0, 0 };
	DirectX::XMFLOAT3 _scale = { 1, 1, 1 };
	DirectX::XMFLOAT3 _rotation = { 0, 0, 0 };

public:
	explicit TransformComponent(std::weak_ptr<::GameObject> owningGameObject, const std::weak_ptr<TransformComponent>& parent = {}) : ComponentBase(owningGameObject)
	{
		WorldMatrix = {};
		Parent = parent;
	}

    void Update() override;
    void FixedUpdate() override;

	void SetPosition(DirectX::XMFLOAT3 pos);
	void SetPosition(float x, float y, float z);
	void AddToPosition(DirectX::XMFLOAT3 pos);
	void AddToPosition(float x, float y, float z);

	void SetScale(DirectX::XMFLOAT3 scale);
	void SetScale(float x, float y, float z);

	void SetRotation(DirectX::XMFLOAT3 rot);
	void SetRotation(float x, float y, float z);
	void AddToRotation(DirectX::XMFLOAT3 rot);
	void AddToRotation(float x, float y, float z);

	[[nodiscard]] DirectX::XMFLOAT3& GetPosition();
	[[nodiscard]] DirectX::XMFLOAT3& GetScale(bool recursiveBlock = false);

	[[nodiscard]] DirectX::XMFLOAT3& GetRotation();
	[[nodiscard]] DirectX::XMFLOAT3& GetWorldPosition();
};

