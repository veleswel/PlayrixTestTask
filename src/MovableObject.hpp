#pragma once
#include "GameObject.hpp"

class MovableObject: public GameObject
{
public:
	virtual ~MovableObject();

protected:
	MovableObject();

public:
	void SetDirectionAngle(float angle);
	float GetDirectionAngle() const;

protected:
	float _directionAngle;
};

