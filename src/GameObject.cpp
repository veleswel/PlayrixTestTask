#include "stdafx.h"
#include "GameObject.hpp"

GameObject::GameObject()
	: _texture(nullptr)
	, _angle(0.f)
	, _scale(1.f)
	, _position(FPoint(0.f, 0.f))
	, _anchorPoint(FPoint(.5f, .5f))
	, _anchorPointTransform(FPoint(0.f, 0.f))
{
	
}

GameObject::~GameObject()
{
	_texture.reset();
}

void GameObject::Init(const std::string& textureName, const FPoint& position, float rotation)
{
	_texture = Core::resourceManager.Get<Render::Texture>(textureName);

	UpdateAnchorPointTransform();
	
	_position = position;
	_angle = rotation;
}

void GameObject::Draw()
{
	Render::device.PushMatrix();

	Render::device.MatrixTranslate(_position);
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), _angle);
	Render::device.MatrixScale(_scale);
	Render::device.MatrixTranslate(_anchorPointTransform);

	_texture->Draw();

	Render::device.PopMatrix();
}

void GameObject::SetPosition(const FPoint& position)
{
	if (_position != position)
	{
		_position = position;
	}
}

void GameObject::SetPosition(float x, float y)
{
	if (_position.x != x && _position.y != y)
	{
		_position.x = x;
		_position.y = y;
	}
}

const FPoint& GameObject::GetPosition() const
{
	return _position;
}

// Точка привязки текстуры указывает на то, как рендерить текстуру относительно позиции объекта.
// Варианты занчений [(0.0;0.0)...(1.0;1.0)], где (0.0;0.0) - левый нижний угол, (1.0;1.0) - правый верхний.
// По дефолту у всех объектов точка привязки (0.5;0.5), т.е. центр текстуры совпадает с позицией объекта

void GameObject::SetAnchorPoint(const FPoint& point)
{
	if (_anchorPoint != point)
	{
		_anchorPoint = point;
		UpdateAnchorPointTransform();
	}
}

const FPoint& GameObject::GetAnchorPoint() const
{
	return _anchorPoint;
}

void GameObject::SetRotationAngle(float angle)
{
	if (_angle != angle)
	{
		_angle = angle;
	}
}

float GameObject::GetRotationAngle() const
{
	return _angle;
}

void GameObject::SetScale(float scale)
{
	if (_scale != scale)
	{
		_scale = scale;
		UpdateAnchorPointTransform();		
	}
}

float GameObject::GetScale() const
{
	return _scale;
}

const FRect GameObject::GetOriginalTextureRect() const
{
	return FRect(_texture->getBitmapRect());
}

const FRect GameObject::GetScaledTextureRect() const
{
	return FRect(_texture->getBitmapRect()).Scaled(_scale);
}

void GameObject::UpdateAnchorPointTransform()
{
	const IRect textureRect = _texture->getBitmapRect();
	_anchorPointTransform.x = -textureRect.Width() * _anchorPoint.x * _scale;
	_anchorPointTransform.y = -textureRect.Height() * _anchorPoint.y * _scale;
}
