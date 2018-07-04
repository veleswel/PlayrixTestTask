#pragma once
#include "GameObject.hpp"

class Cannon;
typedef std::unique_ptr<Cannon> CannonPtr;

class Cannon: public GameObject
{
public:
	static CannonPtr create();
	
	Cannon();
	virtual ~Cannon();

public:
	virtual void Update(float dt) override;
	
protected:
	static const std::string CannonTextureName;
};
