#pragma once
#include "BufferPair.h"
#include "CBLighting.h"
#include "CBMaterial.h"
#include "CBObjectCameraData.h"
#include "CBTextures.h"
#include "SRVBufferPair.h"
#include "SRVDirectionalLights.h"
#include "SRVPointLights.h"
#include "SRVSpotLights.h"
#include "CBExtraData.h"

struct Buffers
{
	inline static BufferPair<CBObjectCameraData> CBObjectCameraData = {}; //Constant Buffer Slot 0
	inline static BufferPair<CBMaterial> CBMaterial = {}; //Constant Buffer Slot 1
	inline static BufferPair<CBTextures> CBTextures = {}; //Constant Buffer Slot 2
	inline static BufferPair<CBLighting> CBLighting = {}; //Constant Buffer Slot 3
	inline static BufferPair<CBExtraData> CBExtraData = {}; //Constant Buffer Slot 4

	inline static SRVBufferPair<SRVDirectionalLights> SRVDirectionalLights = {}; //Texture Buffer Slot 8
	inline static SRVBufferPair<SRVPointLights> SRVPointLights = {}; //Texture Buffer Slot 9
	inline static SRVBufferPair<SRVSpotLights> SRVSpotLights = {}; //Texture Buffer Slot 10
};
