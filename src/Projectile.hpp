#pragma once
#include "MovableObject.hpp"

class Projectile;
typedef std::shared_ptr<Projectile> ProjectilePtr;

class Projectile: public MovableObject
{
public:
	Projectile(const FPoint& position, float rotation, const math::Vector3& velocity, float speed);
	virtual ~Projectile();

protected:
	virtual EColliderType GetColliderType() const override;

protected:
	static const std::string ProjectileTextureName;
};
