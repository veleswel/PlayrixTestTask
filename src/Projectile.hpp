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
	Projectile();
	virtual ~Projectile();
};

typedef boost::intrusive_ptr<Projectile> ProjectilePtr;
