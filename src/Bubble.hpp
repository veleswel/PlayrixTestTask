#pragma once
#include "MovableObject.hpp"

class Bubble: public MovableObject
{
public:
	static Bubble* Create(float speed);
	virtual ~Bubble();

protected:
	Bubble();
	
public:
	virtual void OnCollideWithScreenBorder(const FRect& screenRect) override;
	
protected:
	static const std::string BubbleTextureName;
};

typedef boost::intrusive_ptr<Bubble> BubblePtr;
