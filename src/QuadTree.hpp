#pragma once
#include "OBB.hpp"
#include <bitset>

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
	virtual const OBB2D GetOBB() const = 0;

	virtual EColliderType GetColliderType() const = 0;
};

typedef std::shared_ptr<CollideableDelegate> CollideableDelegatePtr;

class QuadTree
{
public:
	QuadTree(int level, const FRect& bounds);
	~QuadTree();

	void Insert(const CollideableDelegatePtr& object);

	void Retrieve(std::list<CollideableDelegatePtr>& returnObjects, const FRect& rect);
	void Retrieve(std::list<CollideableDelegatePtr>& returnObjects, const FRect& rect, EColliderType mask);

	void Clear();
	
protected:
	void Split();
	int GetIndex(const FRect& rect);
	bool InsertInChild(const CollideableDelegatePtr& object);

protected:
	static const int MaxObjects;
	static const int MaxLevels;

protected:
	int _level;
	
	std::list<CollideableDelegatePtr> _objects;
	
	FRect _bounds;
	
	std::array<std::unique_ptr<QuadTree>, 4> _nodes;

	bool _hasChildren;
};