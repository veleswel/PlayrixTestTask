#include "stdafx.h"
#include "Bubble.hpp"

const std::string Bubble::BubbleTextureName = "bubble";

Bubble::Bubble(const FPoint& position, float rotation, const math::Vector3& velocity, float speed)
{
	Init(BubbleTextureName, position, rotation, velocity, speed);
}

Bubble::~Bubble()
{
	
}

EColliderType Bubble::GetColliderType() const
{
	return EColliderType::EBubble;
}

float Bubble::GetRadius() const
{
	auto corners = _obb.GetCorners();
	return (corners[0] - corners[3]).Length() / 2;
}
