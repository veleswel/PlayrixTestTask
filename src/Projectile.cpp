//
//  Projectile.cpp
//  Test
//
//  Created by Alexey Vlasenko on 6/30/18.
//

#include "stdafx.h"
#include "Projectile.hpp"

const std::string Projectile::ProjectileTextureName = "Star";

Projectile* Projectile::Create()
{
	Projectile* projectile = new (std::nothrow) Projectile();
	projectile->Init(ProjectileTextureName);
	return projectile;
}

Projectile::Projectile()
	:_directionAngle(0.f)
{

}

Projectile::~Projectile()
{

}

void Projectile::SetDirectionAngle(float angle)
{
	if (_directionAngle != angle)
	{
		_directionAngle = angle;
	}
}

float Projectile::GetDirectionAngle() const
{
	return _directionAngle;
}