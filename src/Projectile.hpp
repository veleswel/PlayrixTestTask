#pragma once
#include "MovableObject.hpp"

class Projectile: public MovableObject
{
public:
	static Projectile* Create();
	virtual ~Projectile();

protected:
	Projectile();

protected:
	static const std::string ProjectileTextureName;
};

typedef boost::intrusive_ptr<Projectile> ProjectilePtr;
