#include "stdafx.h"
#include "Bubble.hpp"

const std::string Bubble::BubbleTextureName = "bubble";

Bubble::Bubble(const FPoint& position, float rotation, const FPoint& direction, float speed)
{
	Init(BubbleTextureName, position, rotation, direction, speed);
}

Bubble::~Bubble()
{
	
}

CollisionUtils::EColliderType Bubble::GetColliderType() const
{
	return CollisionUtils::EColliderType::EBubble;
}

// Метод, который рассчитывает радиус. Используется для для определения столновений с другими

float Bubble::GetRadius() const
{
	const auto corners = _obb.GetCorners();
	return (corners[0] - corners[3]).Length() / 2.f;
}
