#pragma once
#include "GameObject.hpp"

class Cannon: public GameObject
{
public:
	static Cannon* create();
	virtual ~Cannon();

protected:
	static const std::string CannonTextureName;
};

typedef boost::intrusive_ptr<Cannon> CannonPtr;