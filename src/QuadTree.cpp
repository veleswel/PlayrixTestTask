#include "stdafx.h"
#include "QuadTree.hpp"

const int QuadTree::MaxObjects = 10;
const int QuadTree::MaxLevels = 5;

static const int IndexNotFound = -1;

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

void QuadTree::Clear()
{
	_hasChildren = false;
	_objects.clear();

	for (auto& node: _nodes)
	{
		node.reset();
	}
}

void QuadTree::Insert(const CollideableDelegatePtr& object)
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

	const auto predicate = [this](const CollideableDelegatePtr& object)
	{
		return InsertInChild(object);
	};

	_objects.erase(std::remove_if(_objects.begin(), _objects.end(), predicate), _objects.end());
}

void QuadTree::Remove(const CollideableDelegatePtr& object)
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

void QuadTree::Retrieve(std::list<CollideableDelegatePtr>& returnObjects, const CollideableDelegatePtr& object)
{
	if (_hasChildren)
	{
		const int index = GetIndex(object->GetAABB());

		if (index != IndexNotFound)
		{
			_nodes[index]->Retrieve(returnObjects, object);
		}
	}

	const auto predicate = [object](const CollideableDelegatePtr& objectToCheck)
	{
		return object != objectToCheck;
	};

	std::copy_if(_objects.begin(), _objects.end(), std::back_inserter(returnObjects), predicate);
}

void QuadTree::Retrieve(std::list<CollideableDelegatePtr>& returnObjects, const CollideableDelegatePtr& object, EColliderType type)
{
	if (_hasChildren)
	{
		const int index = GetIndex(object->GetAABB());

		if (index != IndexNotFound)
		{
			_nodes[index]->Retrieve(returnObjects, object, type);
		}
	}

	const auto predicate = [type, object](const CollideableDelegatePtr& objectToCheck)
	{
		return objectToCheck->GetColliderType() == type && object != objectToCheck;
	};

	std::copy_if(_objects.begin(), _objects.end(), std::back_inserter(returnObjects), predicate);
}

void QuadTree::Split()
{
	const float subWidth = _bounds.Width() / 2.f + 0.f;
	const float subHeight = _bounds.Height() / 2 + 0.f;

	const float x = _bounds.xStart;
	const float y = _bounds.yStart;

	_nodes[0].reset(new QuadTree(_level + 1, FRect(x + subWidth, y, subWidth, subHeight)));
	_nodes[1].reset(new QuadTree(_level + 1, FRect(x, y, subWidth, subHeight)));
	_nodes[2].reset(new QuadTree(_level + 1, FRect(x, y + subHeight, subWidth, subHeight)));
	_nodes[3].reset(new QuadTree(_level + 1, FRect(x + subWidth, y + subHeight, subWidth, subHeight)));

	_hasChildren = true;
}

int QuadTree::GetIndex(const FRect& rect)
{
	const float verticalMidpoint = _bounds.xStart + _bounds.Width() / 2.f;
	const float horizontalMidpoint = _bounds.yStart + _bounds.Height() / 2.f;

	bool top = (rect.yStart + rect.Height() < horizontalMidpoint);
	bool bottom = (rect.yStart > horizontalMidpoint);

	bool left = (rect.xStart + rect.Width() < verticalMidpoint);
	bool right = (rect.xStart > verticalMidpoint);

	if ((top || bottom) && (left || right))
	{
		return (top << 0) | (left << 1);
	}

	return IndexNotFound;
}

bool QuadTree::InsertInChild(const CollideableDelegatePtr& object)
{
	auto index = GetIndex(object->GetAABB());

	if (index == IndexNotFound)
	{
		return false;
	}

	_nodes[index]->Insert(object);
	return true;
}