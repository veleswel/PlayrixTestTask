#include "stdafx.h"
#include "MovableObject.hpp"

MovableObject::MovableObject()
	: _speed(0.f)
{

}

MovableObject::~MovableObject()
{

}

void MovableObject::Init(const std::string& textureName, float speed)
{
	GameObject::Init(textureName);
	_speed = speed;
}

void MovableObject::Draw()
{
	GameObject::Draw();

	Render::device.SetTexturing(false);

	Render::BeginColor(Color(0, 255, 255, 255));

	const OBB2D& obb = GetOBB();
	
	Render::DrawLine(obb._corner[0], obb._corner[1]);
	Render::DrawLine(obb._corner[1], obb._corner[2]);
	Render::DrawLine(obb._corner[2], obb._corner[3]);
	Render::DrawLine(obb._corner[3], obb._corner[0]);

	Render::EndColor();

	Render::device.SetTexturing(true);
}

void MovableObject::Update(float dt)
{
	GameObject::Update(dt);
	UpdatePosition(dt);
}

void MovableObject::SetVelocity(const math::Vector3& velocity)
{
	if (_velocity != math::Vector3::Zero)
	{
		const float angle = (180 * math::GetXYVectorAngle(_velocity, velocity)) / math::PI;
		_angle += angle;
	}
	
	_velocity = velocity;
}

const math::Vector3& MovableObject::GetVelocity() const
{
	return _velocity;
}

OBB2D MovableObject::GetOBB() const
{
	const math::Vector3 position(_position.x, _position.y, 0);
	const FRect texture = GetTextureRect();
	const float angle = (_angle * math::PI) / 180.f;
	
	return OBB2D(position, texture.Width(), texture.Height(), angle);
}

void MovableObject::UpdatePosition(float dt)
{
	float dx = _position.x + _velocity.x * _speed;
	float dy = _position.y + _velocity.y * _speed;

	SetPosition(math::lerp(_position.x, dx, dt), math::lerp(_position.y, dy, dt));
}
