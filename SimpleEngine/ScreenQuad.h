#pragma once
#include <wrl/client.h>

#include "Mesh.h"

class ScreenQuad
{
public:
	ScreenQuad(const Microsoft::WRL::ComPtr<ID3D11Device>& device);

	void Draw(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

private:
	Mesh _mesh;
};

