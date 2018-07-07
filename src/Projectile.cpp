#include "stdafx.h"
#include "Projectile.hpp"

const std::string Projectile::ProjectileTextureName = "projectile";

Projectile::Projectile(const FPoint& position, float rotation, const FPoint& direction, float speed)
{
	Init(ProjectileTextureName, position, rotation, direction, speed);
}

Projectile::~Projectile()
{
	
}

CollisionUtils::EColliderType Projectile::GetColliderType() const
{
	return CollisionUtils::EColliderType::EProjectile;
}
