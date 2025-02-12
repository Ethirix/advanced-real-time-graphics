#pragma once
#include <DirectXMath.h>

struct CBExtraData
{
	DirectX::XMFLOAT3 ColourEffect { 1, 1, 1 };
	unsigned ColourMode;

	unsigned BlurIterations;
	unsigned BlurSampleSize;

	float FocalDepth;
	float FocalBlendDistance;
};
