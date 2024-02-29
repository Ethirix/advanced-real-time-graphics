#include "TransformComponent.h"

#include "Buffers.h"

#pragma region Scale
DirectX::XMFLOAT3 TransformComponent::GetScale(const bool recursiveBlock)
{
	if (Parent.expired() || recursiveBlock)
		return _scale.ToDXFloat3();

	const Vector3 parentScale = Parent.lock()->GetScale(true);
	Vector3 realScale =
	{
		_scale.X / parentScale.X,
		_scale.Y / parentScale.Y,
		_scale.Z / parentScale.Z
	};

	return realScale.ToDXFloat3();
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
DirectX::XMFLOAT3 TransformComponent::GetWorldPosition()
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

DirectX::XMFLOAT3 TransformComponent::GetPosition()
{
	if (Parent.expired())
		return _localPosition.ToDXFloat3();

	const Vector3 parentScale = Parent.lock()->GetScale(true);

	DirectX::XMFLOAT3 realPosition =
	{
		_localPosition.X / parentScale.X,
		_localPosition.Y / parentScale.Y,
		_localPosition.Z / parentScale.Z
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
	_localPosition.X += pos.x;
	_localPosition.Y += pos.y;
	_localPosition.Z += pos.z;
}

void TransformComponent::AddToPosition(float x, float y, float z)
{
	_localPosition.X += x;
	_localPosition.Y += y;
	_localPosition.Z += z;
}


#pragma endregion

#pragma region Rotation
void TransformComponent::SetRotation(const DirectX::XMFLOAT3 rot)
{
	DirectX::XMFLOAT4 q;
	XMStoreFloat4(&q, 
	              DirectX::XMQuaternionRotationRollPitchYawFromVector(
		              XMLoadFloat3(&rot)));

	_quaternion = q;
}

void TransformComponent::SetRotation(float x, float y, float z)
{
	DirectX::XMFLOAT4 q;
	XMStoreFloat4(&q, 
		DirectX::XMQuaternionRotationRollPitchYaw(x, y, z));

	_quaternion = q;
}

void TransformComponent::SetRotation(Quaternion q)
{
	_quaternion = q;
}

void TransformComponent::AddToRotation(DirectX::XMFLOAT3 rot)
{
	DirectX::XMFLOAT4 q = _quaternion.ToDXFloat4();
	XMStoreFloat4(&q, DirectX::XMQuaternionMultiply(XMLoadFloat4(&q), 
		DirectX::XMQuaternionRotationRollPitchYawFromVector(
		XMLoadFloat3(&rot))));

	_quaternion = q;
}

void TransformComponent::AddToRotation(float x, float y, float z)
{
	DirectX::XMFLOAT4 q = _quaternion.ToDXFloat4();
	XMStoreFloat4(&q, DirectX::XMQuaternionMultiply(XMLoadFloat4(&q),
		DirectX::XMQuaternionRotationRollPitchYaw(x, y, z)));

	_quaternion = q;
}

DirectX::XMFLOAT4 TransformComponent::GetRotation()
{
	return _quaternion.ToDXFloat4();
}
#pragma endregion

void TransformComponent::Update(double deltaTime)
{
	DirectX::XMFLOAT4 quaternion = _quaternion.ToDXFloat4();
	DirectX::XMMATRIX matrix = DirectX::XMMatrixScaling(GetScale().x, GetScale().y, GetScale().z)
		* DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&quaternion))
		* DirectX::XMMatrixTranslation(GetPosition().x, GetPosition().y, GetPosition().z);

	if (!Parent.expired())
		matrix *= XMLoadFloat4x4(&Parent.lock()->WorldMatrix);

	XMStoreFloat4x4(&WorldMatrix, matrix);
}
