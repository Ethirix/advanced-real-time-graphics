#pragma once
#include <wrl/client.h>

#include "ShaderType.h"

template <typename T>
class ShaderData
{
public:
	ShaderData(Microsoft::WRL::ComPtr<T> shader, ShaderType type) : Type(type), Shader(std::move(shader)) { }
	ShaderData(Microsoft::WRL::ComPtr<T> shader, ShaderType type, int buffers) : Type(type), Shader(shader) { }

	ShaderType Type;
	Microsoft::WRL::ComPtr<T> Shader = {};
};