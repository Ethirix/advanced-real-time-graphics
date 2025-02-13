#pragma once
#include "Spline.h"

class SplineManager
{
public:
	void AddSpline(Spline spline)
	{
		_splines.emplace_back(spline);
	}

	void Update(float deltaTime)
	{
		if (_splines.empty())
			return;

		_u += deltaTime;
		if (_u > 1)
		{
			_u = 0;
			_i < _splines.size() - 1 ? _i++ : _i = 0;
		}

		_splines[_i].Update(_u);
	}

private:
	float _u = 0;
	unsigned _i = 0;

	std::vector<Spline> _splines{};
};
