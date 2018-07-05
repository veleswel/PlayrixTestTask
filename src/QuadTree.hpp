#pragma once
#include "ColliderDelegate.hpp"

class QuadTree;
typedef std::unique_ptr<QuadTree> QuadTreePtr;
	
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
	std::array<QuadTreePtr, 4> _nodes;
	bool _hasChildren;
};
