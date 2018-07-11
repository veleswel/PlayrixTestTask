#pragma once
#include "GameObject.hpp"
#include "OBB.hpp"
#include "CollisionUtils.hpp"

/* Базовый класс для всех постоянно движущихся объектов в игре (снаряды, пузыри).
Имеет направление движения и постоянную скорость. 
Так же участвует в столкновениях с другими объектами. */

class MovableObject: public GameObject
{
public:
	virtual ~MovableObject();

protected:
	MovableObject();
	
public:
	virtual void Update(float dt) override;

	const FPoint GetVelocity(float dt) const;

	virtual void SetDirection(const FPoint& direction);
	const FPoint& GetDirection() const;

	const OBB& GetOBB() const;
	const FRect GetAABB() const;
	
	float GetSpeed() const;

	virtual CollisionUtils::EColliderType GetColliderType() const = 0;

	void UpdatePosition(float dt);

	void SetCollided(bool isCollided);
	bool IsCollided() const;

	virtual float GetOBBRotationAngle() const = 0;

protected:
	virtual void Init(const std::string& textureName, const FPoint& position, float rotation, const FPoint& direction, float speed);
	
	void UpdateOBB();
	
protected:
	float _speed;

	FPoint _direction;

	OBB _obb;

	bool _isCollided;
};

typedef boost::intrusive_ptr<MovableObject> MovableObjectPtr;
