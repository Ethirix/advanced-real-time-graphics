#pragma once
#include <DirectXMath.h>
#include <string>

#include "Shaders.h"

class Material
{
public:
	std::string Name;
	DirectX::XMFLOAT4 Ambient; //Ka
	DirectX::XMFLOAT4 Diffuse; //Kd
	DirectX::XMFLOAT4 Specular; //Ks
	float SpecularExponent; //Ns
	float Transparency; //d

	Shaders Shader;
};
