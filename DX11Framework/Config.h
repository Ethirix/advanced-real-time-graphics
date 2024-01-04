#pragma once
#include "CameraConfig.h"
#include "ObjectManipulationConfig.h"
#include "Config/GameSceneConfig.h"

struct Config
{
	CameraConfig CameraConfig = {};
	ObjectManipulationConfig ObjectManipulationConfig = {};
	GameSceneConfig GameSceneConfig = {};
};
