#include "stdafx.h"
#include "Projectile.hpp"

const std::string Projectile::ProjectileTextureName = "projectile";

Projectile::Projectile(const FPoint& position, float rotation, const math::Vector3& velocity, float speed)
{
	Init(ProjectileTextureName, position, rotation, velocity, speed);
}

Projectile::~Projectile()
{
	
}

EColliderType Projectile::GetColliderType() const
{
	return EColliderType::EProjectile;
}
