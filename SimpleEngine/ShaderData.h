#pragma once
#include <bitset>
#include <wrl/client.h>

#include "Constants.h"
#include "ShaderType.h"

template <typename T>
class ShaderData
{
public:
	ShaderData(Microsoft::WRL::ComPtr<T> shader, ShaderType type, int buffers) : Type(type), Shader(shader), Buffers(buffers) { }

	ShaderType Type;
	Microsoft::WRL::ComPtr<T> Shader = {};
	std::bitset<MAX_BUFFERS> Buffers = {};
};
