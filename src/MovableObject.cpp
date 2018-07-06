#include "stdafx.h"
#include "MovableObject.hpp"

MovableObject::MovableObject()
	: _speed(0.f)
{

}

MovableObject::~MovableObject()
{

}

void MovableObject::Init(const std::string& textureName, const FPoint& position, float rotation, const math::Vector3& velocity, float speed)
{
	GameObject::Init(textureName, position, rotation);
	
	_velocity = velocity;
	_speed = speed;

	UpdateOBB();
}

void MovableObject::Draw()
{
	GameObject::Draw();

	Render::device.SetTexturing(false);

	const auto& corner = GetOBB().GetCorners();
	
	Render::BeginColor(Color(0, 255, 255, 255));
	Render::DrawLine(corner[0], corner[1]);
	Render::DrawLine(corner[1], corner[2]);
	Render::DrawLine(corner[2], corner[3]);
	Render::DrawLine(corner[3], corner[0]);
	Render::EndColor();
	
	const FRect aabb = GetAABB();

	Render::BeginColor(Color(255, 0, 0, 255));
	Render::DrawLine(aabb.LeftBottom(), aabb.LeftTop());
	Render::DrawLine(aabb.LeftTop(), aabb.RightTop());
	Render::DrawLine(aabb.RightTop(), aabb.RightBottom());
	Render::DrawLine(aabb.RightBottom(), aabb.LeftBottom());
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

math::Vector3& MovableObject::GetVelocity()
{
	return _velocity;
}

const OBB2D& MovableObject::GetOBB() const
{
	return _obb;
}

void MovableObject::UpdateOBB()
{
	if (math::IsEqualFloat(_position.x, 0.f) ||
		math::IsEqualFloat(_position.y, 0.f) ||
		_velocity == math::Vector3::Zero)
	{
		return;
	}
	
	const math::Vector3 position(_position.x, _position.y, 0);
	const FRect texture = GetScaledTextureRect();
	const float angle = (_angle * math::PI) / 180.f;
	
	_obb = OBB2D(position, texture.Width(), texture.Height(), angle);
}

const FRect MovableObject::GetAABB() const
{
	const auto& corner = _obb.GetCorners();
	
	const float minX = fmin(fmin(corner[3].x, corner[2].x), fmin(corner[0].x, corner[1].x));
	const float maxX = fmax(fmax(corner[3].x, corner[2].x), fmax(corner[0].x, corner[1].x));
	const float minY = fmin(fmin(corner[3].y, corner[2].y), fmin(corner[0].y, corner[1].y));
	const float maxY = fmax(fmax(corner[3].y, corner[2].y), fmax(corner[0].y, corner[1].y));
	
	return FRect(minX, maxX, minY, maxY);
}

void MovableObject::UpdatePosition(float dt)
{
	float dx = _position.x + _velocity.x * _speed;
	float dy = _position.y + _velocity.y * _speed;

	SetPosition(math::lerp(_position.x, dx, dt), math::lerp(_position.y, dy, dt));

	/*_position.x += _velocity.x * _speed * dt;
	_position.y += _velocity.y * _speed * dt;*/

	UpdateOBB();
}
