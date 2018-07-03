#pragma once
#include "GameObject.hpp"

class MovableObject;
typedef boost::intrusive_ptr<MovableObject> MovableObjectPtr;

enum class EColissionSide
{
	ENone, ELeft, ETop, ERight, EBottom
};

class MovableObject: public GameObject
{
public:
	virtual ~MovableObject();

protected:
	MovableObject();
	
public:
	virtual void Draw();
	virtual void Update(float dt) override;

	void UpdatePosition(float dt);
	
	void SetDirectionAngle(float angle);
	float GetDirectionAngle() const;

	void SetVelocity(const math::Vector3& velocity);
	const math::Vector3& GetVelocity() const;
	void InvertVelocityX();
	void InvertVelocityY();

	float GetSpeed() const;
	void Stop();

	FRect GetBoundingBox() const;

protected:
	void Init(const std::string& textureName, float speed);

protected:
	float _directionAngle;
	float _speed;

	math::Vector3 _velocity;
};