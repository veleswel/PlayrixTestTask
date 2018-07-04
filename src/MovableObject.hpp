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

	const OBB2D GetOBB() const;
	virtual const FRect GetAABB() const override;
	
protected:
	void Init(const std::string& textureName, float speed);
	
	const FRect GetAABB(const FRect& rect, const math::Matrix4& transform) const;
	
protected:
	float _speed;
	math::Vector3 _velocity;
};

class MovableObject;
typedef boost::intrusive_ptr<MovableObject> MovableObjectPtr;
