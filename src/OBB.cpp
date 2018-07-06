#include "stdafx.h"
#include "OBB.hpp"

OBB2D::OBB2D()
{
	
}

OBB2D::OBB2D(const math::Vector3& center, float width, float height, float angle)
{
	math::Vector3 X(math::cos(angle), math::sin(angle), 0);
	math::Vector3 Y(-math::sin(angle), math::cos(angle), 0);

	X *= width / 2;
	Y *= height / 2;

	_corner[0] = center - X - Y;
	_corner[1] = center + X - Y;
	_corner[2] = center + X + Y;
	_corner[3] = center - X + Y;

	ComputeAxes();
}

OBB2D::OBB2D(const OBB2D& other)
{
	operator=(other);
}

OBB2D& OBB2D::operator = (const OBB2D& other)
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

bool OBB2D::Overlaps(const OBB2D& other) const
{
	return Overlaps1Way(other) && other.Overlaps1Way(*this);
}

const std::array<math::Vector3, 4>& OBB2D::GetCorners() const
{
	return _corner;
}

bool OBB2D::Overlaps1Way(const OBB2D& other) const
{
	float t = 0.f;
	
	for (int a = 0; a < 2; ++a)
	{
		t = other._corner[0].DotProduct(_axis[a]);

		float tMin = t;
		float tMax = t;

		for (int c = 1; c < 4; ++c)
		{
			t = other._corner[c].DotProduct(_axis[a]);

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

void  OBB2D::ComputeAxes()
{
	_axis[0] = _corner[1] - _corner[0];
	_axis[1] = _corner[3] - _corner[0];

	for (int a = 0; a < 2; ++a)
	{
		_axis[a] /= _axis[a].LengthSq();
		_origin[a] = _corner[0].DotProduct(_axis[a]);
	}
}
