#pragma once
#include "Camera.h"
#include "Light.h"
#include "LookAtCamera.h"
#include "LookToCamera.h"
#include "RenderObject.h"
#include "Skybox.h"

class GameObjectFactory
{
public:
	static std::shared_ptr<RenderObject> CreateRenderObject(
		const std::string& meshPath,
		MeshType meshType,
		const std::string& texturePath, 
		const ComPtr<ID3D11Device>& device);

	static std::shared_ptr<Skybox> CreateSkybox(
		const ComPtr<ID3D11Device>& device);

	static std::shared_ptr<Camera> CreateCamera(
		DirectX::XMFLOAT3 position,
		DirectX::XMFLOAT3 at,
		DirectX::XMFLOAT3 up,
		const std::shared_ptr<Screen>& screen);

	static std::shared_ptr<LookAtCamera> CreateLookAtCamera(
		DirectX::XMFLOAT3 position,
		DirectX::XMFLOAT3 at,
		DirectX::XMFLOAT3 up,
		const std::shared_ptr<Screen>& screen);

	static std::shared_ptr<LookToCamera> CreateLookToCamera(
		DirectX::XMFLOAT3 position,
		DirectX::XMFLOAT3 at,
		DirectX::XMFLOAT3 up,
		const std::shared_ptr<Screen>& screen);

	static std::shared_ptr<Light> CreateDirectionalLight(
		DirectX::XMFLOAT3 lightDirection,
		DirectX::XMFLOAT3 diffuseColor,
		float diffusePower,
		DirectX::XMFLOAT3 specularColor,
		float specularPower,
		DirectX::XMFLOAT3 ambientColor);

	static std::shared_ptr<Light> CreatePointLight(
		DirectX::XMFLOAT3 lightPosition,
		DirectX::XMFLOAT3 diffuseColor,
		float diffusePower,
		DirectX::XMFLOAT3 specularColor,
		float specularPower,
		DirectX::XMFLOAT3 ambientColor,
		float constantAttenuation,
		float linearAttenuation,
		float quadraticAttention,
		float lightRadius,
		const ComPtr<ID3D11Device>& device);
};
