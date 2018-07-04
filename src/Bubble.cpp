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

EColliderType Bubble::GetColliderType() const
{
	return EColliderType::EBubble;
}