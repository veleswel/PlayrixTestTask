#pragma once
#include "MovableObject.hpp"

class Projectile;
typedef std::shared_ptr<Projectile> ProjectilePtr;

class Projectile: public MovableObject
{
public:
	static ProjectilePtr Create(float speed);

	Projectile();
	virtual ~Projectile();

protected:
	virtual EColliderType GetColliderType() const override;

protected:
	static const std::string ProjectileTextureName;
};
