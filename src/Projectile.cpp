#include "stdafx.h"
#include "Projectile.hpp"

const std::string Projectile::ProjectileTextureName = "projectile";

ProjectilePtr Projectile::Create(float speed)
{
	auto projectile = std::make_shared<Projectile>();
	projectile->Init(ProjectileTextureName, speed);
	return projectile;
}

Projectile::Projectile()
{

}

Projectile::~Projectile()
{
	Log::Debug("Projectile died!");
}

EColliderType Projectile::GetColliderType() const
{
	return EColliderType::EProjectile;
}