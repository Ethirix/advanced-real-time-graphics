#pragma once
#include <string>
#include <Windows.h>

#include "GameObject.h"

class Helpers
{
public:
	static wchar_t* StringToWideString(std::string str);

	static void UpdateTitleFPS(double deltaTime, std::weak_ptr<GameObject> obj, HWND hwnd);

	static inline HWND WindowHandle = nullptr;
};

