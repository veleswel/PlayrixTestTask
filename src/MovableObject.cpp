#include "stdafx.h"
#include "MovableObject.hpp"

MovableObject::MovableObject()
	: _directionAngle(0.f)
{

}

MovableObject::~MovableObject()
{

}

void MovableObject::SetDirectionAngle(float angle)
{
	if (_directionAngle != angle)
	{
		_directionAngle = angle;
	}
}

float MovableObject::GetDirectionAngle() const
{
	return _directionAngle;
}