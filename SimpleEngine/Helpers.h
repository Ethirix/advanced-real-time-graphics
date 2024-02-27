#pragma once
#include <memory>
#include <string>

#include "GameObject.h"

class Helpers
{
public:
	static wchar_t* StringToWideString(std::string str);

	static void UpdateTitleBar(double deltaTime, const std::weak_ptr<GameObject>& obj, Vector3 force, Vector3 posForce, HWND hwnd);
};

