#pragma once
#include "OBB.hpp"

enum class EColliderType: int
{
	EWall = 1 << 0,
	EProjectile = 1 << 1,
	EBubble = 1 << 2
};
	
inline EColliderType operator | (EColliderType lhs, EColliderType rhs)
{
	return static_cast<EColliderType>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

inline EColliderType operator & (EColliderType lhs, EColliderType rhs)
{
	return static_cast<EColliderType>(static_cast<int>(lhs) & static_cast<int>(rhs));
}
	
class CollideableDelegate
{
public:
	virtual ~CollideableDelegate() { }
	
	virtual const FRect GetAABB() const = 0;
	
	virtual const OBB2D& GetOBB() const = 0;
	
	virtual EColliderType GetColliderType() const = 0;
};
	
typedef std::shared_ptr<CollideableDelegate> CollideableDelegatePtr;
