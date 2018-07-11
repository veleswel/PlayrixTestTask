#pragma once
#include "CollisionUtils.hpp"

class MovableObject;
typedef boost::intrusive_ptr<MovableObject> MovableObjectPtr;

class QuadTree;
typedef std::unique_ptr<QuadTree> QuadTreePtr;

/* Класс, реализующий функционал quadtree, которое используется для оптимизации 
процесса определения столкновений. Вместо того, чтоб проверять столкновения брутфорсом, 
используется данный класс, который позволяет ограничить область поиска объектов для проверки столкновения.
Интерфейс имеет 5 методов: добавить объект, удалить, получить все ближайшие, получить только ближайшие 
объекты определенного типа и очистить дерево. */

class QuadTree
{
public:
	QuadTree(int level, const FRect& bounds);
	~QuadTree();

public:
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
