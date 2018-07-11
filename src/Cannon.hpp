#pragma once
#include "GameObject.hpp"

class Cannon: public GameObject
{
public:
	Cannon(const FPoint& position, float rotation);
	virtual ~Cannon();

public:
	virtual void Update(float dt) override;
	
protected:
	static const float MaxCannonAngle;
	static const float MinCannonAngle;
	
	static const std::string CannonTextureName;
};

typedef std::unique_ptr<Cannon> CannonPtr;
