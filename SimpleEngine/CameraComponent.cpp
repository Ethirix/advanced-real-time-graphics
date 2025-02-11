#include "CameraComponent.h"

#include "Buffers.h"
#include "GameObject.h"
#include "Helpers.h"

CameraComponent::CameraComponent(WP_GAMEOBJECT owningGameObject, nlohmann::json json)
	: ComponentBase(owningGameObject)
{
	_fieldOfView = json["FieldOfView"];
	_at = DirectX::XMFLOAT3(
			json["At"]["X"],
			json["At"]["Y"],
			json["At"]["Z"]);
	_up = DirectX::XMFLOAT3(
			json["Up"]["X"],
			json["Up"]["Y"],
			json["Up"]["Z"]);

	_nearDepth = json["NearPlane"];
	_farDepth = json["FarPlane"];
	_movementSpeed = json["MovementSpeed"];
	_rotationSpeed = json["RotationSpeed"];

	_eye = GameObject.lock()->Transform->GetPosition();

	XMStoreFloat4x4(&_view, DirectX::XMMatrixLookToLH(
		XMLoadFloat3(&_eye),
		XMLoadFloat3(&_at),
		XMLoadFloat3(&_up)));

	XMStoreFloat4x4(&_projection,
		DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(_fieldOfView),
			static_cast<float>(Screen::Width) / static_cast<float>(Screen::Height),
			_nearDepth,
			_farDepth));
}

void CameraComponent::Update(double deltaTime)
{
	if (GetForegroundWindow() != Helpers::WindowHandle && !Helpers::ActiveCamera.expired() 
		&& Helpers::ActiveCamera.lock().get() != this)
		return;

	DirectX::XMMATRIX cameraWorld = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_view));

	//0x0001 is per frame (lowest significance bit)
	//0x8000 is every frame (highest significance bit)
	//r[0] - right; r[1] - up; r[2] - front; r[3] - transform
	
	using namespace DirectX;
	if (GetAsyncKeyState('W') & 0x8000)
	{
		//translate (forward) by camera forward * deltaTime
		cameraWorld.r[3] += cameraWorld.r[2] * deltaTime * _movementSpeed;
	}													   
	if (GetAsyncKeyState('S') & 0x8000)					   
	{													   
		cameraWorld.r[3] -= cameraWorld.r[2] * deltaTime * _movementSpeed;
	}													   
	if (GetAsyncKeyState('D') & 0x8000)					   
	{													   
		cameraWorld.r[3] += cameraWorld.r[0] * deltaTime * _movementSpeed;
	}													   
	if (GetAsyncKeyState('A') & 0x8000)					   
	{													   
		cameraWorld.r[3] -= cameraWorld.r[0] * deltaTime * _movementSpeed;
	}													   
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)			   
	{													   
		cameraWorld.r[3] += cameraWorld.r[1] * deltaTime * _movementSpeed;
	}													   
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)			   
	{													   
		cameraWorld.r[3] -= cameraWorld.r[1] * deltaTime * _movementSpeed;
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		XMMATRIX rotMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(cameraWorld.r[0], 
			static_cast<float>(-deltaTime * _rotationSpeed)));
		cameraWorld.r[1] = XMVector3TransformNormal(cameraWorld.r[1], rotMatrix);
		cameraWorld.r[2] = XMVector3TransformNormal(cameraWorld.r[2], rotMatrix);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		XMMATRIX rotMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(cameraWorld.r[0], 
			static_cast<float>(deltaTime * _rotationSpeed)));
		cameraWorld.r[1] = XMVector3TransformNormal(cameraWorld.r[1], rotMatrix);
		cameraWorld.r[2] = XMVector3TransformNormal(cameraWorld.r[2], rotMatrix);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		//rotate around axis
		XMMATRIX rotMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(cameraWorld.r[1], 
			static_cast<float>(deltaTime * _rotationSpeed)));
		//transform the rotating axis by the rotation matrix
		cameraWorld.r[2] = XMVector3TransformNormal(cameraWorld.r[2], rotMatrix);
		cameraWorld.r[0] = XMVector3TransformNormal(cameraWorld.r[0], rotMatrix);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		XMMATRIX rotMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(cameraWorld.r[1], 
			static_cast<float>(-deltaTime * _rotationSpeed)));
		cameraWorld.r[2] = XMVector3TransformNormal(cameraWorld.r[2], rotMatrix);
		cameraWorld.r[0] = XMVector3TransformNormal(cameraWorld.r[0], rotMatrix);
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		XMMATRIX rotMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(cameraWorld.r[2],
			static_cast<float>(deltaTime * _rotationSpeed)));
		cameraWorld.r[1] = XMVector3TransformNormal(cameraWorld.r[1], rotMatrix);
		cameraWorld.r[0] = XMVector3TransformNormal(cameraWorld.r[0], rotMatrix);
	}
	if (GetAsyncKeyState('E') & 0x8000)
	{
		XMMATRIX rotMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(cameraWorld.r[2],
			static_cast<float>(-deltaTime * _rotationSpeed)));
		cameraWorld.r[1] = XMVector3TransformNormal(cameraWorld.r[1], rotMatrix);
		cameraWorld.r[0] = XMVector3TransformNormal(cameraWorld.r[0], rotMatrix);
	}

	XMStoreFloat3(&_eye, cameraWorld.r[3]);
	XMStoreFloat3(&_at, cameraWorld.r[2]);
	XMStoreFloat3(&_up, cameraWorld.r[1]);
	GameObject.lock()->Transform->SetPosition(_eye);
	XMStoreFloat4x4(&_view, XMMatrixInverse(nullptr, cameraWorld));
}