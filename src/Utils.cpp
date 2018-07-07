#include "stdafx.h"
#include "Utils.hpp"

namespace Utils
{
	float DegreeToRadian(float degree)
	{
		return (degree * math::PI) / 180.f;
	}

	float RadianToDegree(float rad)
	{
		return (rad * 180.f) / math::PI;
	}
}
