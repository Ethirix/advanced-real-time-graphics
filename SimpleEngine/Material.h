#pragma once
#include <DirectXMath.h>
#include <string>

#ifndef _DEFERRED_RENDER
#include "Shaders.h"
#endif

class Material
{
public:
	std::string Name;
	DirectX::XMFLOAT4 Ambient = {}; //Ka
	DirectX::XMFLOAT4 Diffuse = {}; //Kd
	DirectX::XMFLOAT4 Specular = {}; //Ks
	float SpecularExponent = 0; //Ns
	float Transparency = 0; //d
#ifndef _DEFERRED_RENDER
	Shaders Shader = {};
#endif
};
