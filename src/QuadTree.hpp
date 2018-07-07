#pragma once
#include "CollisionUtils.hpp"

class MovableObject;
typedef boost::intrusive_ptr<MovableObject> MovableObjectPtr;

class QuadTree;
typedef std::unique_ptr<QuadTree> QuadTreePtr;

class QuadTree
{
public:
	QuadTree(int level, const FRect& bounds);
	~QuadTree();

	void Insert(const MovableObjectPtr& object);
	void Remove(const MovableObjectPtr& object);

	void Retrieve(std::list<MovableObjectPtr>& returnObjects, const MovableObjectPtr& object);
	void Retrieve(std::list<MovableObjectPtr>& returnObjects, const MovableObjectPtr& object, CollisionUtils::EColliderType mask);

	void Clear();
	
protected:
	void Split();

	int GetIndex(const FRect& rect);

	bool InsertInChild(const MovableObjectPtr& object);

protected:
	static const int MaxObjects;
	static const int MaxLevels;
	static const int IndexNotFound;

protected:
	int _level;
	std::list<MovableObjectPtr> _objects;
	FRect _bounds;
	std::array<QuadTreePtr, 4> _nodes;
	bool _hasChildren;
};
