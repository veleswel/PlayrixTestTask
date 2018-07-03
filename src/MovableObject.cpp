#include "stdafx.h"
#include "MovableObject.hpp"

math::Vector4 transformVec4(const math::Matrix4& m, const math::Vector4& v)
{
	const float x = v[0] * m[0] + v[1] * m[4] + v[2] * m[8] + v[3] * m[12];
	const float y = v[0] * m[1] + v[1] * m[5] + v[2] * m[9] + v[3] * m[13];
	const float z = v[0] * m[2] + v[1] * m[6] + v[2] * m[10] + v[3] * m[14];
	const float w = v[0] * m[3] + v[1] * m[7] + v[2] * m[11] + v[3] * m[15];
	
	return math::Vector4(x, y, z, w);
}

FRect GetTransformedBoundingBox(const FRect& rect, const math::Matrix4& transform)
{
	math::Vector4 topLeft(rect.xStart, rect.yEnd, 0, 1);
	math::Vector4 topRight(rect.xEnd, rect.yEnd, 0, 1);
	math::Vector4 bottomLeft(rect.xStart, rect.yStart, 0, 1);
	math::Vector4 bottomRight(rect.xEnd, rect.yStart, 0, 1);
	
	topLeft = transformVec4(transform, topLeft);
	topRight = transformVec4(transform, topRight);
	bottomLeft = transformVec4(transform, bottomLeft);
	bottomRight = transformVec4(transform, bottomRight);
	
	const float minX = fmin(fmin(topLeft.x, topRight.x), fmin(bottomLeft.x, bottomRight.x));
	const float maxX = fmax(fmax(topLeft.x, topRight.x), fmax(bottomLeft.x, bottomRight.x));
	const float minY = fmin(fmin(topLeft.y, topRight.y), fmin(bottomLeft.y, bottomRight.y));
	const float maxY = fmax(fmax(topLeft.y, topRight.y), fmax(bottomLeft.y, bottomRight.y));
	
	return FRect(minX, maxX, minY, maxY);
}

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
	
	FRect bbox = GetBoundingBox();
	
	Render::device.SetTexturing(false);
	
	Render::BeginColor(Color(255, 255, 0, 255));
	
	Render::DrawLine(bbox.LeftBottom(), bbox.LeftTop());
	Render::DrawLine(bbox.LeftTop(), bbox.RightTop());
	Render::DrawLine(bbox.RightTop(), bbox.RightBottom());
	Render::DrawLine(bbox.RightBottom(), bbox.LeftBottom());
	
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

float MovableObject::GetSpeed() const
{
	return _speed;
}

void MovableObject::Stop()
{
	_speed = 0;
}

void MovableObject::UpdatePosition(float dt)
{
	float dx = _position.x + _velocity.x * _speed;
	float dy = _position.y + _velocity.y * _speed;

	SetPosition(math::lerp(_position.x, dx, dt), math::lerp(_position.y, dy, dt));
}

FRect MovableObject::GetBoundingBox() const
{
	FRect textureRect = GetTextureRect();
	
	math::Matrix4 matrixScale = math::Matrix4::Scaling(_scale, _scale, 1.f);
	math::Matrix4 matrixRotation = math::Matrix4::RotationZ((_angle * math::PI) / 180.f);
	math::Matrix4 matrixTranslate = math::Matrix4::Translation(_position.x, _position.y, 0.f);
	math::Matrix4 resultTransform(matrixScale * matrixRotation * matrixTranslate);
	math::Matrix4 matrixTranslateAnchor = math::Matrix4::Translation(_anchorPointTransform.x, _anchorPointTransform.y, 0.f);
	
	return GetTransformedBoundingBox(textureRect, matrixTranslateAnchor * resultTransform);;
}
