#pragma once
#include <d3d11.h>

#include <wrl/client.h>

class LightManager
{
public:
	static void UpdateLights(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

};

