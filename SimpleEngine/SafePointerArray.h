#pragma once

template<class T>
struct SafePointerArray
{
	SafePointerArray() : Elements(nullptr) {}
	SafePointerArray(T* firstElement, unsigned length)
		: Elements(firstElement), Length(length) {}

	~SafePointerArray()
	{
		delete[] Elements;
	}
	T* Elements;
	unsigned Length = 0;
};