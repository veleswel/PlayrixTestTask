//
//  GameObject.cpp
//  Test
//
//  Created by Alexey Vlasenko on 6/30/18.
//

#include "GameObject.hpp"

GameObject::GameObject(const std::string& textureName)
	: _texture(nullptr)
	, _angle(0.f)
	, _scale(1.f)
	, _position(FPoint(0.f, 0.f))
	, _anchorPoint(FPoint(0.f, 0.f))
	, _anchorPointTransform(FPoint(0.f, 0.f))
{
	
}

GameObject::~GameObject()
{
	_texture = nullptr;
}

void GameObject::Init(const std::string& textureName)
{
	_texture = Core::resourceManager.Get<Render::Texture>(textureName);
}

void GameObject::Draw()
{
	Render::device.PushMatrix();
	Render::device.MatrixScale(_scale);
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), _angle);
	Render::device.MatrixTranslate(GetPosition());
	Render::device.MatrixTranslate(_anchorPointTransform);
	_texture->Draw();
	Render::device.PopMatrix();
}

void GameObject::Update(float dt)
{
	// Empty
}

void GameObject::SetPosition(const FPoint& position)
{
	_position = position;
}

void GameObject::SetPosition(float x, float y)
{
	_position.x = x;
	_position.y = y;
}

const FPoint& GameObject::GetPosition() const
{
	return _position;
}

void GameObject::SetAnchorPoint(const FPoint& point)
{
	if (_anchorPoint != point)
	{
		_anchorPoint = point;
		IRect textureRect = _texture->getBitmapRect();
		
		_anchorPointTransform.x = -textureRect.Width() * _anchorPoint.x;
		_anchorPointTransform.y = -textureRect.Height() * _anchorPoint.y;
	}
}

const FPoint& GameObject::GetAnchorPoint() const
{
	return _anchorPoint;
}

void GameObject::SetRotationAngle(float angle)
{
	_angle = angle;
}

float GameObject::GetRotationAngle() const
{
	return _angle;
}

void GameObject::SetScale(float scale)
{
	_scale = scale;
}

float GameObject::GetScale() const
{
	return _scale;
}
