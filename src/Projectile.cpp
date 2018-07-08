#include "stdafx.h"
#include "Projectile.hpp"
#include "Utils.hpp"

const std::string Projectile::ProjectileTextureName = "projectile";

Projectile::Projectile(const FPoint& position, float rotation, const FPoint& direction, float speed)
	: _effect(nullptr)
	, _animation(nullptr)
	, _isDrawn(false)
{
	Init(ProjectileTextureName, position, rotation, direction, speed);
}

Projectile::~Projectile()
{
	if (_effect)
	{
		_effect->Finish();
		_effect = nullptr;
	}
}

void Projectile::Init(const std::string& textureName, const FPoint& position, float rotation, const FPoint& direction, float speed)
{
	MovableObject::Init(textureName, position, rotation, direction, speed);

	_effect = _effectContainer.AddEffect("Iskra");
	_effect->posX = _position.x + 0.f;
	_effect->posY = _position.y + 0.f;
	_effect->Reset();

	//_animation = Render::Animation::Spawn("ProjectileLaunch");
	//_animation->BeginUse();
}

void Projectile::Draw()
{
	_effectContainer.Draw();
	//_animation->Draw();
	MovableObject::Draw();

	_isDrawn = true;
}

void Projectile::Update(float dt)
{
	MovableObject::Update(dt);
	_effectContainer.Update(dt);
	//_animation->Update(dt);
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
	MovableObject::SetDirection(direction);
}

CollisionUtils::EColliderType Projectile::GetColliderType() const
{
	return CollisionUtils::EColliderType::EProjectile;
}
