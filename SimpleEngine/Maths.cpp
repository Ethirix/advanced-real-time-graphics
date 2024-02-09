#include "Maths.h"

float Maths::Max(float f1, float f2)
{
	return f1 > f2 ? f1 : f2;
}

float Maths::Min(float f1, float f2)
{
	return f1 < f2 ? f1 : f2;
}

float Maths::Pow(float v, int p)
{
	float result = 1;
	for (int i = p; i > 0; i--)
		result *= v;
	for (int i = p; i < 0; i++)
		result /= v;

	return result;
}