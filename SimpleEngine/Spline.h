#pragma once
#include <cmath>
#include <memory>

#include "TransformComponent.h"
#include "Vector3.h"

class Spline
{
public:
	Spline(Vector3 p0, Vector3 p1, Vector3 p2, std::weak_ptr<TransformComponent> transform)
		: _p0(p0), _p1(p1), _p2(p2), _transform(transform)
	{
	}

	void Update(float u)
	{
		if (_transform.expired())
			return;

		Vector3 target = (
			  (_p0 * std::pow(1 - u, 2)) 
			+ (_p1 * (2 * u * (1 - u))) 
			+ (_p2 * std::pow(u, 2))
		);

		_transform.lock()->SetPosition(target.ToDXFloat3());
	}

private:
	Vector3 _p0, _p1, _p2;
	std::weak_ptr<TransformComponent> _transform;
};
