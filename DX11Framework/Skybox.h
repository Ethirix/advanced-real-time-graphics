#pragma once
#include "Camera.h"
#include "RenderObject.h"
#include "SkyboxCBuffer.h"

class Skybox final : public RenderObject
{
	friend class GameObjectFactory;
public:
	void Draw(const ComPtr<ID3D11DeviceContext>& context,
		const ComPtr<ID3D11Buffer>& skyboxBuffer, 
		SkyboxCBuffer& skyboxBufferData,
		const std::shared_ptr<Camera>& camera);

	void Update(double deltaTime) override;
private:
	Skybox() : RenderObject(){}
};
