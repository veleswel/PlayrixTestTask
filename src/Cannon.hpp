#pragma once
#include "GameObject.hpp"

class Cannon: public GameObject
{
public:
	static Cannon* create();
	virtual ~Cannon();
	
protected:
	Cannon();
	
public:
	virtual void Update(float dt) override;
	
protected:
	static const std::string CannonTextureName;
};

typedef boost::intrusive_ptr<Cannon> CannonPtr;
