#pragma once

struct Screen
{
	Screen(const int width, const int height)
	: Width(width), Height(height), Fullscreen(false) { }
																																																																						
	int Width, Height;
	bool Fullscreen;
};
