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
	, _isDrawn(false)
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

void Projectile::Init(const std::string& textureName,
					  const FPoint& position,
					  float rotation,
					  const FPoint& direction,
					  float speed,
					  const EffectsContainerPtr& container)
{
	MovableObject::Init(textureName, position, rotation, direction, speed);

	_effect = container->AddEffect("Iskra");
	_effect->posX = _position.x + 0.f;
	_effect->posY = _position.y + 0.f;
	_effect->Reset();
}

void Projectile::Draw()
{
	MovableObject::Draw();
	_isDrawn = true;
}

void Projectile::Update(float dt)
{
	MovableObject::Update(dt);
}

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
