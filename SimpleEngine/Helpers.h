#pragma once
#include <memory>
#include <string>

#include "CameraComponent.h"
#include "GameObject.h"

class Helpers
{
public:
	static inline HWND WindowHandle = nullptr;
	static inline std::weak_ptr<CameraComponent> ActiveCamera = {};

	static wchar_t* StringToWideString(std::string str);

	static void UpdateTitleBar(double deltaTime, const std::weak_ptr<GameObject>& obj, Vector3 force, Vector3 posForce, HWND hwnd);
};

