#pragma once
#include "BufferPair.h"
#include "CBLighting.h"
#include "CBMaterial.h"
#include "CBObjectCameraData.h"
#include "CBTextures.h"

struct Buffers
{
	static BufferPair<CBObjectCameraData> CBObjectCameraData; //Buffer Slot 0
	static BufferPair<CBMaterial> CBMaterial; //Buffer Slot 1;
	static BufferPair<CBTextures> CBTextures; //Buffer Slot 2;
	static BufferPair<CBLighting> CBLighting; //Buffer Slot 3;
};
