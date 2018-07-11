#include "stdafx.h"
#include "QuadTree.hpp"
#include "MovableObject.hpp"

// Максимальное количество объектов в ноде до того, как она поделится
const int QuadTree::MaxObjects = 5;

// Максимальный уровень сабноды
const int QuadTree::MaxLevels = 4;

const int QuadTree::IndexNotFound = -1;

QuadTree::QuadTree(int level, const FRect& bounds)
	: _level(level)
	, _objects()
	, _nodes()
	, _bounds(bounds)
	, _hasChildren(false)
{
	
}

QuadTree::~QuadTree()
{
	Clear();
}

void QuadTree::Insert(const MovableObjectPtr& object)
{
	if (_hasChildren && InsertInChild(object))
	{
		return;
	}

	_objects.push_back(object);

	if (_hasChildren)
	{
		return;
	}

	if (_level == MaxLevels)
	{
		return;
	}

	if (_objects.size() < MaxObjects)
	{
		return;
	}

	Split();

	const auto predicate = [this](const MovableObjectPtr& object)
	{
		return InsertInChild(object);
	};

	_objects.erase(std::remove_if(_objects.begin(), _objects.end(), predicate), _objects.end());
}

void QuadTree::Remove(const MovableObjectPtr& object)
{
	if (_hasChildren)
	{
		const int index = GetIndex(object->GetAABB());

		if (index != IndexNotFound)
		{
			_nodes[index]->Remove(object);
		}
	}

	const auto iter = std::find(_objects.begin(), _objects.end(), object);
	if (iter != _objects.end())
	{
		_objects.erase(iter);
	}
}

void QuadTree::Retrieve(std::list<MovableObjectPtr>& returnObjects, const MovableObjectPtr& object)
{
	if (_hasChildren)
	{
		const int index = GetIndex(object->GetAABB());

		if (index != IndexNotFound)
		{
			_nodes[index]->Retrieve(returnObjects, object);
		}
	}

	const auto predicate = [object](const MovableObjectPtr& objectToCheck)
	{
		/* Указатель на объект, для которого мы запрашиваем ближайшие, 
		 тоже может содержаться в выборке, поэтому делается эту проверку */
		return object != objectToCheck;
	};

	std::copy_if(_objects.begin(), _objects.end(), std::back_inserter(returnObjects), predicate);
}

void QuadTree::Retrieve(std::list<MovableObjectPtr>& returnObjects, const MovableObjectPtr& object, CollisionUtils::EColliderType type)
{
	if (_hasChildren)
	{
		const int index = GetIndex(object->GetAABB());

		if (index != IndexNotFound)
		{
			_nodes[index]->Retrieve(returnObjects, object, type);
		}
	}

	const auto predicate = [type, object](const MovableObjectPtr& objectToCheck)
	{
		/* Проверяем, что объект имеет нужный тип коллайдера */
		return objectToCheck->GetColliderType() == type && object != objectToCheck;
	};

	std::copy_if(_objects.begin(), _objects.end(), std::back_inserter(returnObjects), predicate);
}

void QuadTree::Clear()
{
	_hasChildren = false;
	_objects.clear();

	for (auto& node : _nodes)
	{
		node.reset();
	}
}

/* Метод, который разделяет ноду на 4 сабноды, если количество 
объектов или уровень достигли максимума. */

void QuadTree::Split()
{
	const float subWidth = _bounds.Width() / 2.f;
	const float subHeight = _bounds.Height() / 2.f;

	const float x = _bounds.xStart;
	const float y = _bounds.yStart;

	_nodes[0].reset(new QuadTree(_level + 1, FRect(x + subWidth, y, subWidth, subHeight)));
	_nodes[1].reset(new QuadTree(_level + 1, FRect(x, y, subWidth, subHeight)));
	_nodes[2].reset(new QuadTree(_level + 1, FRect(x, y + subHeight, subWidth, subHeight)));
	_nodes[3].reset(new QuadTree(_level + 1, FRect(x + subWidth, y + subHeight, subWidth, subHeight)));

	_hasChildren = true;
}

/* Возвращает индекс ноды, в которой находится проверяемый объект(его AABB).
Если объект не помещается полностью ни в одну из дочерних нод и является частью 
парент-ноды, то вернет -1. */

int QuadTree::GetIndex(const FRect& rect)
{
	const float verticalMidpoint = _bounds.xStart + _bounds.Width() / 2.f;
	const float horizontalMidpoint = _bounds.yStart + _bounds.Height() / 2.f;

	const bool top = (rect.yStart + rect.Height() < horizontalMidpoint);
	const bool bottom = (rect.yStart > horizontalMidpoint);

	const bool left = (rect.xStart + rect.Width() < verticalMidpoint);
	const bool right = (rect.xStart > verticalMidpoint);

	if ((top || bottom) && (left || right))
	{
		return (top << 0) | (left << 1);
	}

	return IndexNotFound;
}

/* Метод-хелпер, проверяет, можно ли вставить объект в дочернюю ноду 
и вставляет, если можно. */

bool QuadTree::InsertInChild(const MovableObjectPtr& object)
{
	const int index = GetIndex(object->GetAABB());

	if (index == IndexNotFound)
	{
		return false;
	}

	_nodes[index]->Insert(object);
	return true;
}