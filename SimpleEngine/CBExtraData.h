#pragma once
#include <DirectXMath.h>

struct CBExtraData
{
	DirectX::XMFLOAT3 ColourEffect{};
	unsigned ColourMode;

	unsigned BlurIterations;
	unsigned BlurSampleSize;

	float FocalDepth;
	float FocalBlendDistance;
};
