#pragma once
#include "BufferPair.h"
#include "CBLighting.h"
#include "CBMaterial.h"
#include "CBObjectCameraData.h"
#include "CBTextures.h"
#include "SRVLighting.h"
#include "SRVBufferPair.h"

struct Buffers
{
	inline static BufferPair<CBObjectCameraData> CBObjectCameraData = {}; //Constant Buffer Slot 0
	inline static BufferPair<CBMaterial> CBMaterial = {}; //Constant Buffer Slot 1
	inline static BufferPair<CBTextures> CBTextures = {}; //Constant Buffer Slot 2
	inline static BufferPair<CBLighting> CBLighting = {}; //Constant Buffer Slot 3

	inline static SRVBufferPair<SRVLighting> SRVLighting = {}; //Texture Buffer Slot 8
};
