#include "stdafx.h"
#include "Projectile.hpp"

const std::string Projectile::ProjectileTextureName = "projectile";

Projectile* Projectile::Create(float speed)
{
	Projectile* projectile = new (std::nothrow) Projectile();
	projectile->Init(ProjectileTextureName, speed);
	return projectile;
}

Projectile::Projectile()
{

}

Projectile::~Projectile()
{
	
}

EColliderType Projectile::GetColliderType() const
{
	return EColliderType::EProjectile;
}