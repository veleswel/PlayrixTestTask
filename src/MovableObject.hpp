#pragma once
#include "GameObject.hpp"
#include "QuadTree.hpp"
#include "OBB.hpp"

class MovableObject: public GameObject, public CollideableDelegate
{
public:
	virtual ~MovableObject();

protected:
	MovableObject();
	
public:
	virtual void Draw() override;
	virtual void Update(float dt) override;
	
	void UpdatePosition(float dt);

	void SetVelocity(const math::Vector3& velocity);
	const math::Vector3& GetVelocity() const;

	virtual const OBB2D& GetOBB() const override;
	virtual const FRect GetAABB() const override;
	
	float GetSpeed() const { return _speed; }

	float GetAcceleration() const;
	void SetAcceleration(float acc);

protected:
	void Init(const std::string& textureName, const FPoint& position, float rotation, const math::Vector3& velocity, float speed);
	
	void UpdateOBB();
	
protected:
	float _speed;
	float _acceleration;
	math::Vector3 _velocity;
	OBB2D _obb;
};

typedef std::shared_ptr<MovableObject> MovableObjectPtr;
