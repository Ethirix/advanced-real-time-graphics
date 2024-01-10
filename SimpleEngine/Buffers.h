#pragma once
#include "CBLighting.h"
#include "CBMaterial.h"
#include "CBObjectCameraData.h"
#include "CBTextures.h"

struct Buffers
{
	static CBObjectCameraData CBObjectCameraData; //Buffer Slot 0
	static CBMaterial CBMaterial; //Buffer Slot 1;
	static CBTextures CBTextures; //Buffer Slot 2;
	static CBLighting CBLighting; //Buffer Slot 3;
};
