#pragma once
#include <wrl/client.h>

#include "Mesh.h"

class ScreenQuad
{
public:
	ScreenQuad(const Microsoft::WRL::ComPtr<ID3D11Device>& device);

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv);

private:
	Mesh _mesh;
};

