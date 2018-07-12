#pragma once
#include "GameObject.hpp"
#include "OBB.hpp"
#include "CollisionUtils.hpp"

// Базовый класс для всех объектов в игре, которые постоянно движутся (шары и снаряды)
// Содержит скорость объекта, вектор направления движения, OBB. 
// Принимает участие в рассчете и разрешении столкновений с другими объектами на сцене

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

protected:
	virtual void Init(const std::string& textureName, const FPoint& position, float rotation, const FPoint& direction, float speed);
	
	void UpdateOBB();
	
protected:
	float _speed;

	FPoint _direction;

	// OBB реализован в качестве мембера класса, так как он пересчитывается на каждой итерации цикла.
	// И проще это сделать один раз(если не было столкновений), чем рассчитыват его каждый раз по запросу
	// других

	OBB _obb;

	bool _isCollided;
};

typedef boost::intrusive_ptr<MovableObject> MovableObjectPtr;
