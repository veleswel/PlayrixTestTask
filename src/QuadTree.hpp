#pragma once
#include "CollisionUtils.hpp"

class MovableObject;
typedef boost::intrusive_ptr<MovableObject> MovableObjectPtr;

class QuadTree;
typedef std::unique_ptr<QuadTree> QuadTreePtr;

// This class implements quadtree funcional that is used for search optimization during collision detection.
// The interface has 4 methods: add an object to the tree, remove an object, retrieve all objects near to specific object and clear the tree

class QuadTree
{
public:
	QuadTree(int level, const FRect& bounds);
	~QuadTree();

public:
	void Insert(const MovableObjectPtr& object);
	void Remove(const MovableObjectPtr& object);

	void Retrieve(std::list<MovableObjectPtr>& returnObjects, const MovableObjectPtr& object, CollisionUtils::EColliderType type);

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
