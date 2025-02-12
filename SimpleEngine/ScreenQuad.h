#pragma once
#include <wrl/client.h>

#include "Mesh.h"

class ScreenQuad
{
public:
	ScreenQuad(const Microsoft::WRL::ComPtr<ID3D11Device>& device, float size = 1.0f, DirectX::XMFLOAT2 offset = {0, 0});

	void Draw(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

private:
	Mesh _mesh;
};

