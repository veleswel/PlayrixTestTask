#pragma once
#include "GameObject.hpp"

class MovableObject: public GameObject
{
public:
	virtual ~MovableObject();

protected:
	MovableObject();
	
public:
	virtual void Draw();
	virtual void Update(float dt) override;
	
	void SetDirectionAngle(float angle);
	float GetDirectionAngle() const;

	float GetSpeed() const;
	void Stop();
	
	bool IsCollidingWithScreenBorder(const FRect& screenRect) const;
	virtual void OnCollideWithScreenBorder(const FRect& screenRect);

protected:
	void Init(const std::string& textureName, float speed);

	FRect GetBoundingBox() const;
	
protected:
	float _directionAngle;
	float _speed;
};

