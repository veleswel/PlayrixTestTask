#pragma once
#include "MovableObject.hpp"

class Bubble;
typedef std::shared_ptr<Bubble> BubblePtr;

class Bubble: public MovableObject
{
public:
	Bubble(const FPoint& position, float rotation, const math::Vector3& velocity, float speed);
	virtual ~Bubble();

public:
	virtual EColliderType GetColliderType() const override;

	float GetRadius() const;
	
protected:
	static const std::string BubbleTextureName;
};
