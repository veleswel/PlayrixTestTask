#include "stdafx.h"
#include "Cannon.hpp"
#include "Utils.hpp"

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
	GameObject::Update(dt);
	
	FPoint mousePosition = static_cast<FPoint>(Core::mainInput.GetMousePos());
	
	if (mousePosition.x == 0.f && mousePosition.y == 0)
	{
		return;
	}
	
	const float angle = Utils::RadianToDegree(math::atan(mousePosition.y - _position.y, mousePosition.x - _position.x));
	if (angle >= MinCannonAngle && angle <= MaxCannonAngle)
	{
		_angle = angle;
	}
}