#pragma once
#include <d3d11.h>
#include <wrl/client.h>

template <typename T>
struct SRVBufferPair
{
	T BufferData;
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Resource;
};
