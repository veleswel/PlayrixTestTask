#include "stdafx.h"
#include "MovableObject.hpp"
#include "Utils.hpp"
#include "MainSceneWidget.hpp"

MovableObject::MovableObject()
	: _speed(0.f)
	, _direction(0.f, 0.f)
{

}

MovableObject::~MovableObject()
{

}

void MovableObject::Init(const std::string& textureName, const FPoint& position, float rotation, const FPoint& direction, float speed)
{
	GameObject::Init(textureName, position, rotation);
	
	_direction = direction;
	_speed = speed;

	UpdateOBB();
}

/* �� ������ �������� �������� ����� ��������� ������� �������. */

void MovableObject::Update(float dt)
{
	UpdatePosition(dt);
}

/* ����� ���������� ������ �������� ������� � ������� ������ �������, 
�.�. ����� ��������� ������ ���������� �� ����� dt. */

const FPoint MovableObject::GetVelocity(float dt) const
{
	return _direction * _speed * dt;
}

void MovableObject::SetDirection(const FPoint& direction)
{
	_direction = direction;
}

const FPoint& MovableObject::GetDirection() const
{
	return _direction;
}

const OBB& MovableObject::GetOBB() const
{
	return _obb;
}

/* ����� ������������ AABB(axis-aligned bounding box) ��� �������, ��������� ��� OBB. 
������������ � ������ QuadTree ��� ��������� ������������ �� �����. */

const FRect MovableObject::GetAABB() const
{
	const auto& corner = _obb.GetCorners();
	
	const float minX = fmin(fmin(corner[3].x, corner[2].x), fmin(corner[0].x, corner[1].x));
	const float maxX = fmax(fmax(corner[3].x, corner[2].x), fmax(corner[0].x, corner[1].x));
	const float minY = fmin(fmin(corner[3].y, corner[2].y), fmin(corner[0].y, corner[1].y));
	const float maxY = fmax(fmax(corner[3].y, corner[2].y), fmax(corner[0].y, corner[1].y));
	
	return FRect(minX, maxX, minY, maxY);
}

float MovableObject::GetSpeed() const
{
	return _speed;
}

/* ������� ������ � ��� �����������. ������������ �������� ������������, 
���� ������� �������� ����� �������. ����� ����, ��� ������ ���������, 
��������� ��� OBB. */

void MovableObject::UpdatePosition(float dt)
{
	float dx = _position.x + _direction.x * _speed;
	float dy = _position.y + _direction.y * _speed;

	SetPosition(math::lerp(_position.x, dx, dt), math::lerp(_position.y, dy, dt));

	UpdateOBB();
}

void MovableObject::SetCollided(bool isCollided)
{
	_isCollided = isCollided;
}

bool MovableObject::IsCollided() const
{
	return _isCollided;
}

/* � ������ ���������� OBB �������� � �������� ������� ������, 
������ ��� ������ ��������� ��������� � �� ������ �������� ����� ��� OBB ���������� ���������. */

void MovableObject::UpdateOBB()
{
	if (math::IsEqualFloat(_position.x, 0.f) ||
		math::IsEqualFloat(_position.y, 0.f))
	{
		return;
	}

	const FRect texture = GetScaledTextureRect();
	_obb = OBB(_position, texture.Width(), texture.Height(), Utils::DegreeToRadian(GetOBBRotationAngle()));
}