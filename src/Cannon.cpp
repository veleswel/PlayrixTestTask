#include "stdafx.h"
#include "Cannon.hpp"

const std::string Cannon::CannonTextureName = "cannon_2";

CannonPtr Cannon::create()
{
	auto cannon = std::make_unique<Cannon>();
	cannon->Init(CannonTextureName);
	return cannon;
}

Cannon::Cannon()
{
	
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
	
	_angle = (math::atan(mousePosition.y - _position.y, mousePosition.x - _position.x) * 180.f) / math::PI;
}
