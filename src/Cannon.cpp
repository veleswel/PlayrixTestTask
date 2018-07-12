#include "stdafx.h"
#include "Cannon.hpp"
#include "Utils.hpp"

// Значения минимального и максимального угла наклона для пушки

const float Cannon::MaxCannonAngle = 157.f;
const float Cannon::MinCannonAngle = 23.f;

const std::string Cannon::CannonTextureName = "cannon";

Cannon::Cannon(const FPoint& position, float rotation)
{
	Init(CannonTextureName, position, rotation);
}

Cannon::~Cannon()
{
	
}

void Cannon::Update(float dt)
{
	const FPoint mousePosition = static_cast<FPoint>(Core::mainInput.GetMousePos());
	
	if (mousePosition.x == 0.f && mousePosition.y == 0)
	{
		return;
	}
	
	// Проверяем, что угол наклона находится в заданной области

	const float angle = Utils::RadianToDegree(math::atan(mousePosition.y - _position.y, mousePosition.x - _position.x));
	if (angle >= MinCannonAngle && angle <= MaxCannonAngle)
	{
		_angle = angle;
	}
}
