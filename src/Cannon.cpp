#include "stdafx.h"
#include "Cannon.hpp"

const std::string Cannon::CannonTextureName = "cannon_2";

Cannon* Cannon::create()
{
	Cannon* cannon = new (std::nothrow) Cannon();
	cannon->Init(CannonTextureName);
	return cannon;
}

Cannon::~Cannon()
{
	
}