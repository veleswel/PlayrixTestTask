//
//  GameObject.hpp
//  Test
//
//  Created by Alexey Vlasenko on 6/30/18.
//

#pragma once

class GameObject: public RefCounter
{
public:
	GameObject(const std::string& textureName);
	virtual ~GameObject();
	
protected:
	GameObject(const GameObject& gameObject) = delete;
	GameObject& operator=(const GameObject& gameObject) = delete;
	
public:
	virtual void Draw();
	virtual void Update(float dt);
	
	void SetPosition(const FPoint& position);
	void SetPosition(float x, float y);
	const FPoint& GetPosition() const;
	
	void SetAnchorPoint(const FPoint& point);
	const FPoint& GetAnchorPoint() const;
	
	void SetRotationAngle(float angle);
	float GetRotationAngle() const;
	
	void SetScale(float scale);
	float GetScale() const;
	
protected:
	virtual void Init(const std::string& textureName);
	
protected:
	Render::Texture* _texture;
	
	FPoint _position;
	
	float _angle;
	
	float _scale;
	
	FPoint _anchorPoint;
	FPoint _anchorPointTransform;
};