#include "CameraComponent.h"

#include "Buffers.h"
#include "GameObject.h"

CameraComponent::CameraComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json)
	: ComponentBase(owningGameObject)
{
	_fieldOfView = json["FieldOfView"];
	_nearDepth = json["NearPlane"];
	_farDepth = json["FarPlane"];
	_movementSpeed = json["MovementSpeed"];
	_rotationSpeed = json["RotationSpeed"];

	DirectX::XMFLOAT3 pos = GameObject.lock()->Transform->GetPosition();
	DirectX::XMFLOAT3 rot = MakeEulerAnglesFromQ(GameObject.lock()->Transform->GetRotation()).ToDXFloat3();

	DirectX::XMFLOAT3 forward = GetForward(rot);
	DirectX::XMFLOAT3 up = GetUp(rot);

	XMStoreFloat4x4(&_view, DirectX::XMMatrixLookToLH(
		XMLoadFloat3(&pos),
		XMLoadFloat3(&forward),
		XMLoadFloat3(&up)));

	XMStoreFloat4x4(&_projection,
		DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(_fieldOfView),
			static_cast<float>(Screen::Width) / static_cast<float>(Screen::Height),
			_nearDepth,
			_farDepth));
}

DirectX::XMFLOAT3 CameraComponent::GetForward(DirectX::XMFLOAT3 rot)
{
	if (GameObject.expired())
		return { 0, 0, 1 };
	
	return {
		cos(DegreesToRadians(rot.x)) * sin(DegreesToRadians(rot.y)),
		-sin(DegreesToRadians(rot.x)),
		cos(DegreesToRadians(rot.x)) * cos(DegreesToRadians(rot.y))
	};
}

DirectX::XMFLOAT3 CameraComponent::GetRight(DirectX::XMFLOAT3 rot)
{
	if (GameObject.expired())
		return { 0, 0, 1 };

	return {
		cos(DegreesToRadians(rot.y)),
		0,
		-sin(DegreesToRadians(rot.y))
	};
}

DirectX::XMFLOAT3 CameraComponent::GetUp(DirectX::XMFLOAT3 rot)
{
	if (GameObject.expired())
		return { 0, 0, 1 };

	return {
		sin(DegreesToRadians(rot.x)) * sin(DegreesToRadians(rot.y)),
		cos(DegreesToRadians(rot.x)),
		sin(DegreesToRadians(rot.x)) * cos(DegreesToRadians(rot.y))
	};
}

void CameraComponent::Update(double deltaTime)
{
	if (GameObject.expired())
		return;

	//0x0001 is per frame (lowest significance bit)
	//0x8000 is every frame (highest significance bit)
	std::shared_ptr<TransformComponent> transform = GameObject.lock()->Transform;

	using namespace DirectX;
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		transform->AddToRotation(-deltaTime * _rotationSpeed, 0, 0);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		transform->AddToRotation(deltaTime * _rotationSpeed, 0, 0);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		transform->AddToRotation(0, deltaTime * _rotationSpeed, 0);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		transform->AddToRotation(0, -deltaTime * _rotationSpeed, 0);
	}
	//if (GetAsyncKeyState('Q') & 0x8000)
	//{
	//	transform->AddToRotation(0, 0, -deltaTime * _rotationSpeed);
	//}
	//if (GetAsyncKeyState('E') & 0x8000)
	//{
	//	transform->AddToRotation(0, 0, deltaTime * _rotationSpeed);
	//}

	XMFLOAT3 rot = MakeEulerAnglesFromQ(GameObject.lock()->Transform->GetRotation()).ToDXFloat3();
	XMFLOAT3 pos = GameObject.lock()->Transform->GetPosition();
	XMVECTOR posV = XMLoadFloat3(&pos);

	XMFLOAT3 forward = GetForward(rot);
	XMVECTOR forwardV = XMLoadFloat3(&forward);

	XMFLOAT3 right = GetRight(rot);
	XMVECTOR rightV = XMLoadFloat3(&right);

	XMFLOAT3 up = GetUp(rot);
	XMVECTOR upV = XMLoadFloat3(&up);
	
	if (GetAsyncKeyState('W') & 0x8000)
	{
		posV += forwardV * deltaTime * _movementSpeed;
	}													   
	if (GetAsyncKeyState('S') & 0x8000)					   
	{													   
		posV -= forwardV * deltaTime * _movementSpeed;
	}													   
	if (GetAsyncKeyState('D') & 0x8000)					   
	{													   
		posV += rightV * deltaTime * _movementSpeed;
	}													   
	if (GetAsyncKeyState('A') & 0x8000)					   
	{													   
		posV -= rightV * deltaTime * _movementSpeed;
	}													   
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)			   
	{													   
		posV += upV * deltaTime * _movementSpeed;
	}													   
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)			   
	{													   
		posV -= upV * deltaTime * _movementSpeed;
	}

	XMStoreFloat3(&pos, posV);
	XMStoreFloat4x4(&_view, XMMatrixLookToLH(posV, forwardV, upV));

	transform->SetPosition(pos);

	Buffers::CBObjectCameraData.BufferData.CameraPosition = { pos.x, pos.y, pos.z, 1 };
	Buffers::CBObjectCameraData.BufferData.View = XMMatrixTranspose(XMLoadFloat4x4(&_view));
	Buffers::CBObjectCameraData.BufferData.Projection = XMMatrixTranspose(XMLoadFloat4x4(&GetProjection()));
}