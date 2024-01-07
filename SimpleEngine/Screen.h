#pragma once

struct Screen
{
	Screen(int width, int height, bool fullscreen = false)
		: Width(width), Height(height), Fullscreen(fullscreen) {}

	int Width, Height;
	bool Fullscreen;
};
