#pragma once
#include <string>
#include <wrl/client.h>

struct Texture
{
	std::string Name = "";
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Resource {};
	int Slot = -1;
};
