#pragma once
#include <d3d11.h>
#include <wrl/client.h>

template <typename T>
struct BufferPair
{
	T BufferData;
	ComPtr<ID3D11Buffer> Buffer;
};
