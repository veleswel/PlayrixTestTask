//
//  Projectile.hpp
//  Test
//
//  Created by Alexey Vlasenko on 6/30/18.
//

#pragma once
#include "GameObject.hpp"

class Projectile: public GameObject
{
public:
	static Projectile* Create();
	virtual ~Projectile();

protected:
	Projectile();

public:
	void SetDirectionAngle(float angle);
	float GetDirectionAngle() const;

protected:
	static const std::string ProjectileTextureName;

protected:
	float _directionAngle;
};

typedef boost::intrusive_ptr<Projectile> ProjectilePtr;
