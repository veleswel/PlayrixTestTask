#include "stdafx.h"
#include "Projectile.hpp"
#include "Utils.hpp"

const std::string Projectile::ProjectileTextureName = "projectile";

Projectile::Projectile(const FPoint& position, float rotation, const FPoint& direction, float speed)
{
	Init(ProjectileTextureName, position, rotation, direction, speed);
}

Projectile::~Projectile()
{
	
}

void  Projectile::SetDirection(const FPoint& direction)
{
	if (!math::IsEqualFloat(direction.x, 0.f) && !math::IsEqualFloat(direction.y, 0.f))
	{
		const float angle = Utils::RadianToDegree(_direction.GetDirectedAngle(direction));
		_angle += angle;
		while (_angle > 360.f)
		{
			_angle -= 360.f;
		}
	}

	MovableObject::SetDirection(direction);
}

CollisionUtils::EColliderType Projectile::GetColliderType() const
{
	return CollisionUtils::EColliderType::EProjectile;
}
