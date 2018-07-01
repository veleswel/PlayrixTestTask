#include "stdafx.h"
#include "Projectile.hpp"

const std::string Projectile::ProjectileTextureName = "projectile";

Projectile* Projectile::Create()
{
	Projectile* projectile = new (std::nothrow) Projectile();
	projectile->Init(ProjectileTextureName);
	return projectile;
}

Projectile::Projectile()
{

}

Projectile::~Projectile()
{

}