#pragma once
#include <DirectXMath.h>
#include <string>

#include "Shaders.h"

class Material
{
public:
	std::string Name;
	DirectX::XMFLOAT4 Ambient = {}; //Ka
	DirectX::XMFLOAT4 Diffuse = {}; //Kd
	DirectX::XMFLOAT4 Specular = {}; //Ks
	float SpecularExponent = 0; //Ns
	float Transparency = 0; //d

	Shaders Shader = {};
};
