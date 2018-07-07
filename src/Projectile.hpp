#pragma once
#include "MovableObject.hpp"

class Projectile: public MovableObject
{
public:
	Projectile(const FPoint& position, float rotation, const FPoint& direction, float speed);
	virtual ~Projectile();

protected:
	virtual CollisionUtils::EColliderType GetColliderType() const override;

protected:
	static const std::string ProjectileTextureName;
};

typedef boost::intrusive_ptr<Projectile> ProjectilePtr;
