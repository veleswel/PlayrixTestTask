#pragma once
#include "CollisionUtils.hpp"

class MovableObject;
typedef boost::intrusive_ptr<MovableObject> MovableObjectPtr;

class QuadTree;
typedef std::unique_ptr<QuadTree> QuadTreePtr;

// Класс, реализующий функционал quadtree, который используется для оптимизации поиска объектов при определении столкновений.
// Интерфейс реализует 4 метода: добавить объект в дерево, удалить объект, получить все объекты рядом с проверяемым объектом 
// и очистить дерево

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
