#include "stdafx.h"
#include "Bubble.hpp"

const std::string Bubble::BubbleTextureName = "bubble";

BubblePtr Bubble::Create(float speed)
{
	auto bubble = std::make_shared<Bubble>();
	bubble->Init(BubbleTextureName, speed);
	return bubble;
}

Bubble::Bubble()
{

}

Bubble::~Bubble()
{
	Log::Debug("Bubble died!");
}

EColliderType Bubble::GetColliderType() const
{
	return EColliderType::EBubble;
}