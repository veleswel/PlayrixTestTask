#pragma once
#include "OBB.hpp"

enum class EColliderType
{
	EWall,
	EProjectile,
	EBubble
};
		
class CollideableDelegate
{
public:
	CollideableDelegate() : _isCollided(false) { }
	virtual ~CollideableDelegate() { }
	
	virtual const FRect GetAABB() const = 0;
	
	virtual const OBB2D& GetOBB() const = 0;
	
	virtual EColliderType GetColliderType() const = 0;

	void SetCollided(bool isCollided) { _isCollided = isCollided;  }
	bool isCollided() const { return _isCollided;  }

protected:
	bool _isCollided;
};
	
typedef std::shared_ptr<CollideableDelegate> CollideableDelegatePtr;
