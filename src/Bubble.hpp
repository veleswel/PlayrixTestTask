#pragma once
#include "MovableObject.hpp"

class Bubble: public MovableObject
{
public:
	Bubble(const FPoint& position, float rotation, const FPoint& direction, float speed);
	virtual ~Bubble();

public:
	virtual CollisionUtils::EColliderType GetColliderType() const override;

	float GetRadius() const;
	
	virtual float GetOBBRotationAngle() const override;

protected:
	static const std::string BubbleTextureName;
};

typedef boost::intrusive_ptr<Bubble> BubblePtr;