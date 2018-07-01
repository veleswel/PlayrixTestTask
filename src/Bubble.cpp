#include "stdafx.h"
#include "Bubble.hpp"

const std::string Bubble::BubbleTextureName = "bubble";

Bubble* Bubble::Create()
{
	Bubble* bubble = new (std::nothrow) Bubble();
	bubble->Init(BubbleTextureName);
	return bubble;
}

Bubble::Bubble()
{

}

Bubble::~Bubble()
{

}