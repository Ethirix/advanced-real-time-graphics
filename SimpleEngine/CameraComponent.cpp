#include "CameraComponent.h"

#include "GameObject.h"

CameraComponent::CameraComponent(WP_GAMEOBJECT owningGameObject,
                                 float fov, DirectX::XMFLOAT3 at, DirectX::XMFLOAT3 up, 
								 float nearPlane, float farPlane,
                                 float movementSpeed, float rotationSpeed)
: ComponentBase(owningGameObject), _at(at), _up(up),
	_nearDepth(nearPlane), _farDepth(farPlane), _fieldOfView(fov)
{
	_eye = GameObject.lock().get()->Transform->GetPosition();

	XMStoreFloat4x4(&_view, DirectX::XMMatrixLookAtLH(
		                XMLoadFloat3(&_eye),
		                XMLoadFloat3(&_at),
		                XMLoadFloat3(&_up)));

	XMStoreFloat4x4(&_projection, 
		DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(_fieldOfView),
		static_cast<float>(Screen::Width) / static_cast<float>(Screen::Height),
			_nearDepth, _farDepth));
}

void CameraComponent::Update(double deltaTime)
{
	
}

