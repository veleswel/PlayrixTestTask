#pragma once
#include "MovableObject.hpp"

class Bubble;
typedef std::shared_ptr<Bubble> BubblePtr;

class Bubble: public MovableObject
{
public:
	static BubblePtr Create(float speed);

	Bubble();
	virtual ~Bubble();

public:
	virtual EColliderType GetColliderType() const override;

protected:
	static const std::string BubbleTextureName;
};
