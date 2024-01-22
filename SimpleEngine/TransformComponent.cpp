#include "TransformComponent.h"

#pragma region Scale
DirectX::XMFLOAT3& TransformComponent::GetScale(const bool recursiveBlock)
{
	if (Parent.expired() || recursiveBlock)
		return _scale;

	const DirectX::XMFLOAT3 parentScale = Parent.lock()->GetScale(true);
	DirectX::XMFLOAT3 realScale =
	{
		_scale.x / parentScale.x,
		_scale.y / parentScale.y,
		_scale.z / parentScale.z
	};

	return realScale;
}

void TransformComponent::SetScale(const DirectX::XMFLOAT3 scale)
{
	_scale = scale;
}

void TransformComponent::SetScale(float x, float y, float z)
{
	_scale = { x, y, z };
}
#pragma endregion

#pragma region Position
DirectX::XMFLOAT3& TransformComponent::GetWorldPosition()
{
	DirectX::XMVECTOR transform;
	DirectX::XMVECTOR rot;
	DirectX::XMVECTOR scale;
	const DirectX::XMMATRIX worldMatrix = XMLoadFloat4x4(&WorldMatrix);

	XMMatrixDecompose(&scale, &rot, &transform, worldMatrix);

	DirectX::XMFLOAT3 pos;
	XMStoreFloat3(&pos, transform);

	return pos;
}

DirectX::XMFLOAT3& TransformComponent::GetPosition()
{
	if (Parent.expired())
		return _localPosition;

	const DirectX::XMFLOAT3 parentScale = Parent.lock()->GetScale(true);

	DirectX::XMFLOAT3 realPosition =
	{
		_localPosition.x / parentScale.x,
		_localPosition.y / parentScale.y,
		_localPosition.z / parentScale.z
	};

	return realPosition;
}

void TransformComponent::SetPosition(const DirectX::XMFLOAT3 pos)
{
	_localPosition = pos;
}

void TransformComponent::SetPosition(float x, float y, float z)
{
	_localPosition = { x, y, z };
}

void TransformComponent::AddToPosition(DirectX::XMFLOAT3 pos)
{
	_localPosition.x += pos.x;
	_localPosition.y += pos.y;
	_localPosition.z += pos.z;
}

void TransformComponent::AddToPosition(float x, float y, float z)
{
	_localPosition.x += x;
	_localPosition.y += y;
	_localPosition.z += z;
}


#pragma endregion

#pragma region Rotation
void TransformComponent::SetRotation(const DirectX::XMFLOAT3 rot)
{
	_rotation = rot;
}

void TransformComponent::SetRotation(float x, float y, float z)
{
	_rotation = { x, y, z };
}

void TransformComponent::AddToRotation(DirectX::XMFLOAT3 rot)
{
	_rotation = {
		_rotation.x + rot.x,
		_rotation.y + rot.y,
		_rotation.z + rot.z
	};
}

void TransformComponent::AddToRotation(float x, float y, float z)
{
	_rotation = {
		_rotation.x + x,
		_rotation.y + y,
		_rotation.z + z
	};
}


DirectX::XMFLOAT3& TransformComponent::GetRotation()
{
	return _rotation;
}
#pragma endregion