#pragma once
#include "MovableObject.hpp"

class Bubble: public MovableObject
{
public:
	static Bubble* Create(float speed);
	virtual ~Bubble();

protected:
	Bubble();
	
protected:
	static const std::string BubbleTextureName;
};

typedef boost::intrusive_ptr<Bubble> BubblePtr;
