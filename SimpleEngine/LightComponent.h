#pragma once
#include <DirectXMath.h>

#include "ComponentBase.h"
#include "GameObject.h"

class LightComponent : public ComponentBase
{
protected:
	explicit LightComponent(WP_GAMEOBJECT owningGameObject) : ComponentBase(owningGameObject) {}

public:
	virtual ~LightComponent() = 0;

	DirectX::XMFLOAT3 DiffuseColor	= {};
	float DiffusePower				= {};
	DirectX::XMFLOAT3 SpecularColor = {};
	float SpecularPower				= {};
	DirectX::XMFLOAT3 AmbientColor	= {};
};