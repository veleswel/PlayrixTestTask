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

const OBB2D MovableObject::GetOBB() const
{
	const math::Vector3 position(_position.x, _position.y, 0);
	const FRect texture = GetTextureRect();
	const float angle = (_angle * math::PI) / 180.f;
	
	return OBB2D(position, texture.Width(), texture.Height(), angle);
}

const FRect MovableObject::GetAABB() const
{
	FRect textureRect = GetTextureRect();
	const math::Vector3 center(_position.x, _position.y, 0);

	const FRect texture = GetTextureRect();

	const math::Matrix4 matrixScale = math::Matrix4::Scaling(_scale, _scale, 1.f);
	const math::Matrix4 matrixRotation = math::Matrix4::RotationZ((_angle * math::PI) / 180.f);
	const math::Matrix4 matrixTranslate = math::Matrix4::Translation(_position.x, _position.y, 0.f);
	const math::Matrix4 resultTransform(matrixScale * matrixRotation * matrixTranslate);
	const math::Matrix4 matrixTranslateAnchor = math::Matrix4::Translation(_anchorPointTransform.x, _anchorPointTransform.y, 0.f);

	return GetAABB(textureRect, matrixTranslateAnchor * resultTransform);
}

const FRect MovableObject::GetAABB(const FRect& rect, const math::Matrix4& transform) const
{
	math::Vector4 leftTop(rect.xStart, rect.yEnd, 0, 1);
	math::Vector4 rightTop(rect.xEnd, rect.yEnd, 0, 1);
	math::Vector4 leftBottom(rect.xStart, rect.yStart, 0, 1);
	math::Vector4 rightBottom(rect.xEnd, rect.yStart, 0, 1);
	
	leftTop = leftTop.Transform(transform);
	rightTop = rightTop.Transform(transform);
	leftBottom = leftBottom.Transform(transform);
	rightBottom = rightBottom.Transform(transform);
	
	const float minX = fmin(fmin(leftTop.x, rightTop.x), fmin(leftBottom.x, rightBottom.x));
	const float maxX = fmax(fmax(leftTop.x, rightTop.x), fmax(leftBottom.x, rightBottom.x));
	const float minY = fmin(fmin(leftTop.y, rightTop.y), fmin(leftBottom.y, rightBottom.y));
	const float maxY = fmax(fmax(leftTop.y, rightTop.y), fmax(leftBottom.y, rightBottom.y));
	
	return FRect(minX, maxX, minY, maxY);
}

void MovableObject::UpdatePosition(float dt)
{
	float dx = _position.x + _velocity.x * _speed;
	float dy = _position.y + _velocity.y * _speed;

	SetPosition(math::lerp(_position.x, dx, dt), math::lerp(_position.y, dy, dt));
}
