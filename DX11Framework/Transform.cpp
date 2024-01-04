#include "Transform.h"

#include <memory>

#include "RenderObject.h"

#pragma region Scale
DirectX::XMFLOAT3& Transform::GetScale(const bool recursiveBlock)
{
	const auto parent = GameObject.lock()->GetParent();
	if (!parent || recursiveBlock)
		return _scale;

	const DirectX::XMFLOAT3 parentScale = parent->Transform->GetScale(true);
	DirectX::XMFLOAT3 realScale =
	{
		_scale.x / parentScale.x,
		_scale.y / parentScale.y,
		_scale.z / parentScale.z
	};

	return realScale;
}

void Transform::SetScale(const DirectX::XMFLOAT3 scale)
{
	_scale = scale;
}

void Transform::SetScale(float x, float y, float z)
{
	_scale = { x, y, z };
}
#pragma endregion

#pragma region Position
DirectX::XMFLOAT3& Transform::GetWorldPosition()
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

DirectX::XMFLOAT3& Transform::GetPosition()
{
	const auto parent = GameObject.lock()->GetParent();
	if (!parent)
		return _localPosition;

	const DirectX::XMFLOAT3 parentScale = parent->Transform->GetScale(true);

	DirectX::XMFLOAT3 realPosition =
	{
		_localPosition.x / parentScale.x,
		_localPosition.y / parentScale.y,
		_localPosition.z / parentScale.z
	};

	return realPosition;
}

void Transform::SetPosition(const DirectX::XMFLOAT3 pos)
{
	_localPosition = pos;
}

void Transform::SetPosition(float x, float y, float z)
{
	_localPosition = { x, y, z };
}

void Transform::AddToPosition(DirectX::XMFLOAT3 pos)
{
	_localPosition.x += pos.x;
	_localPosition.y += pos.y;
	_localPosition.z += pos.z;
}

void Transform::AddToPosition(float x, float y, float z)
{
	_localPosition.x += x;
	_localPosition.y += y;
	_localPosition.z += z;
}


#pragma endregion
void Transform::SetRotation(const DirectX::XMFLOAT3 rot)
{
	_rotation = rot;
}

void Transform::SetRotation(float x, float y, float z)
{
	_rotation = { x, y, z };
}

void Transform::AddToRotation(DirectX::XMFLOAT3 rot)
{
	_rotation = {
		_rotation.x + rot.x,
		_rotation.y + rot.y,
		_rotation.z + rot.z
	};
}

void Transform::AddToRotation(float x, float y, float z)
{
	_rotation = {
		_rotation.x + x,
		_rotation.y + y,
		_rotation.z + z
	};
}


DirectX::XMFLOAT3& Transform::GetRotation()
{
	return _rotation;
}