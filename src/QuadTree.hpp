#pragma once
#include "OBB.hpp"

class MovableObject;
typedef boost::intrusive_ptr<MovableObject> MovableObjectPtr;

enum class EColliderType
{
	EWall, EProjectile, EBubble
};

class CollideableDelegate
{
public:
	virtual ~CollideableDelegate() { }

	virtual const FRect GetAABB() const = 0;
	virtual EColliderType GetColliderType() const = 0;
};

typedef boost::intrusive_ptr<CollideableDelegate> CollideableDelegatePtr;

class QuadTree
{
public:
	QuadTree(int level, const FRect& bounds);
	~QuadTree();

	void Insert(const MovableObjectPtr& object);

	void Retrieve(std::list<MovableObjectPtr>& returnObjects, const FRect& rect);

	void Clear();
	
protected:
	void Split();
	int GetIndex(const FRect& rect);
	bool InsertInChild(const MovableObjectPtr& object);

protected:
	static const int MaxObjects;
	static const int MaxLevels;

protected:
	int _level;
	
	std::list<MovableObjectPtr> _objects;
	
	FRect _bounds;
	
	std::array<std::unique_ptr<QuadTree>, 4> _nodes;

	bool _hasChildren;
};

