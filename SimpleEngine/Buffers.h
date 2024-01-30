#pragma once
#include "BufferPair.h"
#include "CBLighting.h"
#include "CBMaterial.h"
#include "CBObjectCameraData.h"
#include "CBTextures.h"

struct Buffers
{
	inline static BufferPair<CBObjectCameraData> CBObjectCameraData = {}; //Buffer Slot 0
	inline static BufferPair<CBMaterial> CBMaterial = {}; //Buffer Slot 1;
	inline static BufferPair<CBTextures> CBTextures = {}; //Buffer Slot 2;
	inline static BufferPair<CBLighting> CBLighting = {}; //Buffer Slot 3;
};
