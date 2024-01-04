#include "Light.h"

LightData Light::GetLightData()
{
	LightData data = LightData();
	data.Position = DirectX::XMFLOAT4(Transform->GetPosition().x, Transform->GetPosition().y, Transform->GetPosition().z, 0);
	data.DiffuseColor = DiffuseColor;
	data.DiffusePower = DiffusePower;
	data.SpecularColor = SpecularColor;
	data.SpecularPower = SpecularPower;
	data.AmbientColor = AmbientColor;
	data.ConstantAttenuation = ConstantAttenuation;
	data.LinearAttenuation = LinearAttenuation;
	data.QuadraticAttenuation = QuadraticAttenuation;
	data.LightRadius = LightRadius;

	return data;
}