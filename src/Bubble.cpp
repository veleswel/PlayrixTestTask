#include "stdafx.h"
#include "Bubble.hpp"

const std::string Bubble::BubbleTextureName = "bubble";

Bubble* Bubble::Create(float speed)
{
	Bubble* bubble = new (std::nothrow) Bubble();
	bubble->Init(BubbleTextureName, speed);
	return bubble;
}

Bubble::Bubble()
{

}

Bubble::~Bubble()
{

}

void Bubble::OnCollideWithScreenBorder(const FRect& screenRect)
{
	if (_position.x <= 0 || _position.x >= screenRect.xEnd)
	{
		MovableObject::OnCollideWithScreenBorder(screenRect);
	}
	
	if (_position.y <= 0 || _position.y >= screenRect.yEnd)
	{
		SetDirectionAngle(2 * math::PI - _directionAngle);
	}
}
