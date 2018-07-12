#include "stdafx.h"
#include "Projectile.hpp"
#include "Utils.hpp"

const std::string Projectile::ProjectileTextureName = "projectile";

Projectile::Projectile(const FPoint& position,
					   float rotation,
					   const FPoint& direction,
					   float speed,
					   const EffectsContainerPtr& container)
	: _effect(nullptr)
{
	Init(ProjectileTextureName, position, rotation, direction, speed, container);
}

Projectile::~Projectile()
{
	if (_effect)
	{
		_effect->Finish();
		_effect = nullptr;
	}
}

// При инициализации снаряда создаем эффект

void Projectile::Init(const std::string& textureName,
					  const FPoint& position,
					  float rotation,
					  const FPoint& direction,
					  float speed,
					  const EffectsContainerPtr& container)
{
	MovableObject::Init(textureName, position, rotation, direction, speed);

	_effect = container->AddEffect("Bubbles");
	_effect->posX = _position.x + 0.f;
	_effect->posY = _position.y + 0.f;
	_effect->Reset();
}

// При изменении позиции снаряда меняем позицию эффека

void Projectile::SetPosition(const FPoint& position)
{
	MovableObject::SetPosition(position);
	if (_effect)
	{
		_effect->posX = _position.x;
		_effect->posY = _position.y;
	}
}

void Projectile::SetPosition(float x, float y)
{
	MovableObject::SetPosition(x, y);
	if (_effect)
	{
		_effect->posX = _position.x;
		_effect->posY = _position.y;
	}
}

// При изменении вектора направления движения меняем так же угол наклона снаряда

void Projectile::SetDirection(const FPoint& direction)
{
	if (!math::IsEqualFloat(direction.x, 0.f) && !math::IsEqualFloat(direction.y, 0.f))
	{
		// Рассчитываем угол между текущим вектором и новым
		// и прибавляем к текущему углу
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
