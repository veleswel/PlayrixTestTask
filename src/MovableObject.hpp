#pragma once
#include "GameObject.hpp"
#include "OBB.hpp"
#include "CollisionUtils.hpp"

class MovableObject: public GameObject
{
public:
	virtual ~MovableObject();

protected:
	MovableObject();
	
public:
	virtual void Draw() override;
	virtual void Update(float dt) override;
	
	const FPoint GetVelocity(float dt) const;

	void SetDirection(const FPoint& velocity);
	const FPoint& GetDirection() const;

	const OBB2D& GetOBB() const;
	const FRect GetAABB() const;
	
	float GetSpeed() const;

	virtual CollisionUtils::EColliderType GetColliderType() const = 0;

	void UpdatePosition(float dt);

	void SetCollided(bool isCollided);
	bool IsCollided() const;

protected:
	void Init(const std::string& textureName, const FPoint& position, float rotation, const FPoint& direction, float speed);
	
	void UpdateOBB();
	
protected:
	float _speed;

	FPoint _direction;
	FPoint _velocity;

	OBB2D _obb;

	bool _isCollided;
};

typedef boost::intrusive_ptr<MovableObject> MovableObjectPtr;
