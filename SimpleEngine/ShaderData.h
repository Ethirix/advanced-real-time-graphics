#pragma once
#include <bitset>
#include <wrl/client.h>

#include "ShaderType.h"

constexpr int CB_00 = 0b0000'0000'0000'0001; //UNUSED
constexpr int CB_01 = 0b0000'0000'0000'0010; //UNUSED
constexpr int CB_02 = 0b0000'0000'0000'0100; //UNUSED
constexpr int CB_03 = 0b0000'0000'0000'1000; //UNUSED
constexpr int CB_04 = 0b0000'0000'0001'0000; //UNUSED
constexpr int CB_05 = 0b0000'0000'0010'0000; //UNUSED
constexpr int CB_06 = 0b0000'0000'0100'0000; //UNUSED
constexpr int CB_07 = 0b0000'0000'1000'0000; //UNUSED
constexpr int CB_08 = 0b0000'0001'0000'0000; //UNUSED
constexpr int CB_09 = 0b0000'0010'0000'0000; //UNUSED
constexpr int CB_10 = 0b0000'0100'0000'0000; //UNUSED
constexpr int CB_11 = 0b0000'1000'0000'0000; //UNUSED
constexpr int CB_12 = 0b0001'0000'0000'0000; //UNUSED
constexpr int CB_13 = 0b0010'0000'0000'0000; //UNUSED
constexpr int CB_14 = 0b0100'0000'0000'0000; //UNUSED

constexpr int MAX_BUFFERS = 15; //This is the Resource Limit for Constant Buffers

template <typename T>
class ShaderData
{
public:
	ShaderData(Microsoft::WRL::ComPtr<T> shader, ShaderType type, int buffers) : Type(type), Shader(shader), Buffers(buffers) { }

	ShaderType Type;
	Microsoft::WRL::ComPtr<T> Shader = {};
	std::bitset<MAX_BUFFERS> Buffers = {};
};
