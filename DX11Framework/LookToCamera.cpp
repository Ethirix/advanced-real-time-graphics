#include "LookToCamera.h"

#include "ConfigManager.h"
#include "Helpers.h"

void LookToCamera::Update(double deltaTime)
{
	if (GetForegroundWindow() != Helpers::WindowHandle || GetAsyncKeyState(VK_LSHIFT) & 0x8000)
		return;

	DirectX::XMMATRIX cameraWorld = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_view));
	auto config = ConfigManager::GetConfig()->CameraConfig;

	//0x0001 is per frame (lowest significance bit)
	//0x8000 is every frame (highest significance bit)
	//r[0] - right; r[1] - up; r[2] - front; r[3] - transform
	
	using namespace DirectX;
	if (GetAsyncKeyState('W') & 0x8000)
	{
		//translate (forward) by camera forward * deltaTime
		cameraWorld.r[3] += cameraWorld.r[2] * deltaTime * config.MovementMultiplier;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		cameraWorld.r[3] -= cameraWorld.r[2] * deltaTime * config.MovementMultiplier;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		cameraWorld.r[3] += cameraWorld.r[0] * deltaTime * config.MovementMultiplier;
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		cameraWorld.r[3] -= cameraWorld.r[0] * deltaTime * config.MovementMultiplier;
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		cameraWorld.r[3] += cameraWorld.r[1] * deltaTime * config.MovementMultiplier;
	}
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
	{
		cameraWorld.r[3] -= cameraWorld.r[1] * deltaTime * config.MovementMultiplier;
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		XMMATRIX rotMatrix = XMMatrixRotationAxis(cameraWorld.r[0], 
			static_cast<float>(-deltaTime * config.RotationMultiplier));
		cameraWorld.r[1] = XMVector3TransformNormal(cameraWorld.r[1], rotMatrix);
		cameraWorld.r[2] = XMVector3TransformNormal(cameraWorld.r[2], rotMatrix);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		XMMATRIX rotMatrix = XMMatrixRotationAxis(cameraWorld.r[0], 
			static_cast<float>(deltaTime * config.RotationMultiplier));
		cameraWorld.r[1] = XMVector3TransformNormal(cameraWorld.r[1], rotMatrix);
		cameraWorld.r[2] = XMVector3TransformNormal(cameraWorld.r[2], rotMatrix);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		//rotate around axis
		XMMATRIX rotMatrix = XMMatrixRotationAxis(cameraWorld.r[1], 
			static_cast<float>(deltaTime * config.RotationMultiplier));
		//transform the rotating axis by the rotation matrix
		cameraWorld.r[2] = XMVector3TransformNormal(cameraWorld.r[2], rotMatrix);
		cameraWorld.r[0] = XMVector3TransformNormal(cameraWorld.r[0], rotMatrix);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		XMMATRIX rotMatrix = XMMatrixRotationAxis(cameraWorld.r[1], 
			static_cast<float>(-deltaTime * config.RotationMultiplier));
		cameraWorld.r[2] = XMVector3TransformNormal(cameraWorld.r[2], rotMatrix);
		cameraWorld.r[0] = XMVector3TransformNormal(cameraWorld.r[0], rotMatrix);
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		XMMATRIX rotMatrix = XMMatrixRotationAxis(cameraWorld.r[2],
			static_cast<float>(deltaTime * config.RotationMultiplier));
		cameraWorld.r[1] = XMVector3TransformNormal(cameraWorld.r[1], rotMatrix);
		cameraWorld.r[0] = XMVector3TransformNormal(cameraWorld.r[0], rotMatrix);
	}
	if (GetAsyncKeyState('E') & 0x8000)
	{
		XMMATRIX rotMatrix = XMMatrixRotationAxis(cameraWorld.r[2],
			static_cast<float>(-deltaTime * config.RotationMultiplier));
		cameraWorld.r[1] = XMVector3TransformNormal(cameraWorld.r[1], rotMatrix);
		cameraWorld.r[0] = XMVector3TransformNormal(cameraWorld.r[0], rotMatrix);
	}

	XMStoreFloat3(&_eye, cameraWorld.r[3]);
	XMStoreFloat4x4(&_view, XMMatrixInverse(nullptr, cameraWorld));
}
