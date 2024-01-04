#pragma once

#include "RenderObject.h"

class Light final : public RenderObject
{
	friend class GameObjectFactory;
public:
	~Light() override = default;

	DirectX::XMFLOAT3 DiffuseColor, SpecularColor, AmbientColor;
	float DiffusePower, SpecularPower, AmbientPower;
	float ConstantAttenuation = 1, LinearAttenuation, QuadraticAttenuation;
	float LightRadius;

	LightData GetLightData();

private:
	Light() : RenderObject(),
	DiffuseColor(),
	SpecularColor(),
	AmbientColor(),
	DiffusePower(0),
	SpecularPower(0),
	AmbientPower(0),
	LinearAttenuation(0),
	QuadraticAttenuation(0),
	LightRadius(0)
	{
	}
};
