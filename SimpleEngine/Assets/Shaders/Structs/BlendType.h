#pragma once

enum class BlendType : unsigned
{
	None				= 0,
	Darken				= 1,
	Multiply			= 2,
	ColorBurn		= 3,
	LinearBurn		= 4,
	Lighten			= 5,
	Screen				= 6,
	ColorDodge		= 7,
	LinearDodge	= 8,
	Difference		= 9,
	Exclusion			= 10
};