#pragma once
#include "Camera.h"

class LookToCamera final : public Camera
{
	friend class GameObjectFactory;
	LookToCamera() : Camera() { }
public:
	void Update(double deltaTime) override;
};