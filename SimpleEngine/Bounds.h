#pragma once
#include "Vector3.h"

struct Bounds
{
	//Center of Bounds
	Vector3 Center;
	//Center - Extents
	Vector3 Min;
	//Center + Extents
	Vector3 Max;
	//Half size of Bounds (equivalent to Max)
	Vector3 Extents;
	//Twice as long as Extents
	Vector3 Size;
};
