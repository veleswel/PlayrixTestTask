#include "stdafx.h"
#include "MovableObject.hpp"

math::Vector4 transformVec4(const math::Matrix4& m, const math::Vector4& v)
{
	// Handle case where v == dst.
	float x = v[0] * m[0] + v[1] * m[4] + v[2] * m[8] + v[3] * m[12];
	float y = v[0] * m[1] + v[1] * m[5] + v[2] * m[9] + v[3] * m[13];
	float z = v[0] * m[2] + v[1] * m[6] + v[2] * m[10] + v[3] * m[14];
	float w = v[0] * m[3] + v[1] * m[7] + v[2] * m[11] + v[3] * m[15];
	
	math::Vector4 dst;
	
	dst[0] = x;
	dst[1] = y;
	dst[2] = z;
	dst[3] = w;
	
	return dst;
}

FRect RectApplyTransform(const FRect& rect, const math::Matrix4& transform)
{
	float top    = rect.yEnd;
	float left   = rect.xStart;
	float right  = rect.xEnd;
	float bottom = rect.yStart;
	
	math::Vector4 topLeft(left, top, 0, 1);
	math::Vector4 topRight(right, top, 0, 1);
	math::Vector4 bottomLeft(left, bottom, 0, 1);
	math::Vector4 bottomRight(right, bottom, 0, 1);
	
	topLeft = transformVec4(transform, topLeft);
	topRight = transformVec4(transform, topRight);
	bottomLeft = transformVec4(transform, bottomLeft);
	bottomRight = transformVec4(transform, bottomRight);
	
	float minX = fmin(fmin(topLeft.x, topRight.x), fmin(bottomLeft.x, bottomRight.x));
	float maxX = fmax(fmax(topLeft.x, topRight.x), fmax(bottomLeft.x, bottomRight.x));
	float minY = fmin(fmin(topLeft.y, topRight.y), fmin(bottomLeft.y, bottomRight.y));
	float maxY = fmax(fmax(topLeft.y, topRight.y), fmax(bottomLeft.y, bottomRight.y));
	
	return FRect(minX, maxX, minY, maxY);
}

MovableObject::MovableObject()
	: _directionAngle(0.f)
	, _speed(0.f)
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
	
//	Render::DrawRect(bbox.xStart, bbox.yStart, bbox.Width(), bbox.Height());
	
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
	
	_position.x += math::cos(_directionAngle) * (_speed * dt);
	_position.y += math::sin(_directionAngle) * (_speed * dt);
}

void MovableObject::SetDirectionAngle(float angle)
{
	if (_directionAngle != angle)
	{
		_directionAngle = angle;
		
		_angle = (_directionAngle * 180) / math::PI;
	}
}

float MovableObject::GetDirectionAngle() const
{
	return _directionAngle;
}

float MovableObject::GetSpeed() const
{
	return _speed;
}

void MovableObject::Stop()
{
	_speed = 0;
}

bool MovableObject::IsCollidingWithScreenBorder(const FRect& screenRect) const
{
	return false;
}

void MovableObject::OnCollideWithScreenBorder(const FRect& screenRect)
{
	SetDirectionAngle(math::PI - _directionAngle);
}

FRect MovableObject::GetBoundingBox() const
{
	FRect textureRect = GetTextureRect();
	
	math::Matrix4 matrixScale = math::Matrix4::Scaling(_scale, _scale, 1.f);
	math::Matrix4 matrixRotation = math::Matrix4::RotationZ(_directionAngle);
	math::Matrix4 matrixTranslate = math::Matrix4::Translation(_position.x, _position.y, 0.f);
	math::Matrix4 resultTransform(matrixScale * matrixRotation * matrixTranslate);
	
	math::Matrix4 matrixTranslateAnchor = math::Matrix4::Translation(_anchorPointTransform.x, _anchorPointTransform.y, 0.f);
	
	FRect newRect = RectApplyTransform(textureRect, matrixTranslateAnchor * resultTransform);
	
	return newRect;
}
