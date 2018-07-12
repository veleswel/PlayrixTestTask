#include "stdafx.h"
#include "OBB.hpp"

OBB::OBB()
{
	
}

OBB::OBB(const FPoint& center, float width, float height, float angle)
{
	FPoint X(math::cos(angle), math::sin(angle));
	FPoint Y(-math::sin(angle), math::cos(angle));

	X *= width / 2;
	Y *= height / 2;

	_corner[0] = center - X - Y;
	_corner[1] = center + X - Y;
	_corner[2] = center + X + Y;
	_corner[3] = center - X + Y;

	ComputeAxes();
}

OBB::OBB(const OBB& other)
{
	operator=(other);
}

OBB& OBB::operator = (const OBB& other)
{
	_corner[0] = other._corner[0];
	_corner[1] = other._corner[1];
	_corner[2] = other._corner[2];
	_corner[3] = other._corner[3];

	_axis[0] = other._axis[0];
	_axis[1] = other._axis[1];

	_origin[0] = other._origin[0];
	_origin[1] = other._origin[1];

	return *this;
}

bool OBB::Overlaps(const OBB& other) const
{
	return OverlapsOther(other) && other.OverlapsOther(*this);
}

const std::array<FPoint, 4>& OBB::GetCorners() const
{
	return _corner;
}

bool OBB::OverlapsOther(const OBB& other) const
{
	for (int a = 0; a < 2; ++a)
	{
		float t = other._corner[0].GetDotProduct(_axis[a]);

		float tMin = t;
		float tMax = t;

		for (int c = 1; c < 4; ++c)
		{
			t = other._corner[c].GetDotProduct(_axis[a]);

			if (t < tMin)
			{
				tMin = t;
			}
			else if (t > tMax)
			{
				tMax = t;
			}
		}

		if ((tMin > 1.f + _origin[a]) || (tMax < _origin[a]))
		{
			return false;
		}
	}

	return true;
}

void OBB::ComputeAxes()
{
	_axis[0] = _corner[1] - _corner[0];
	_axis[1] = _corner[3] - _corner[0];

	for (int a = 0; a < 2; ++a)
	{
		_axis[a] /= math::sqr(_axis[a].Length());
		_origin[a] = _corner[0].GetDotProduct(_axis[a]);
	}
}
