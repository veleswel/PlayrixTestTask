#pragma once
#include "MovableObject.hpp"

static int MAX_OBJECTS = 10;
static int MAX_LEVELS = 5;

class QuadTreeDelegate
{
public:
	virtual ~QuadTreeDelegate()
	{
		
	}
};

class QuadTree
{
public:
	QuadTree(int level, const FRect& bounds)
		: _level(level)
		, _bounds(bounds)
	{
		for (int i = 0; i < 4; i++)
		{
			_nodes[i] == nullptr;
		}
	}
	
	void Clear()
	{
		_objects.clear();
		for (int i = 0; i < 4; i++)
		{
			if (_nodes[i] != nullptr)
			{
				_nodes[i]->Clear();
				delete _nodes[i];
			}
		}
	}
	
	void Split()
	{
		const float subWidth = _bounds.Width() / 2.f + 0.f;
		const float subHeight = _bounds.Height() / 2 + 0.f;
		
		const float x = _bounds.xStart;
		const float y = _bounds.yStart;
		
		_nodes[0] = new QuadTree(_level + 1, FRect(x + subWidth, y, subWidth, subHeight));
		_nodes[1] = new QuadTree(_level + 1, FRect(x, y, subWidth, subHeight));
		_nodes[2] = new QuadTree(_level + 1, FRect(x, y + subHeight, subWidth, subHeight));
		_nodes[3] = new QuadTree(_level + 1, FRect(x + subWidth, y + subHeight, subWidth, subHeight));
	}
	
	int GetIndex(const MovableObjectPtr& object)
	{
		int index = -1;
		
		const float verticalMidpoint = _bounds.xStart + (_bounds.Width() / 2.f) + 0.f;
		const float horizontalMidpoint = _bounds.yStart + (_bounds.Height() / 2.f) + 0.f;
		
		const FRect rect = object->GetAABB();
		
		// Object can completely fit within the top quadrants
		bool topQuadrant = (rect.yStart < horizontalMidpoint) && (rect.yStart + rect.Height() < horizontalMidpoint);
		
		// Object can completely fit within the bottom quadrants
		bool bottomQuadrant = rect.yStart > horizontalMidpoint;
		
		// Object can completely fit within the left quadrants
		if ((rect.xStart < verticalMidpoint) && (rect.xStart + rect.Width() < verticalMidpoint))
		{
			if (topQuadrant)
			{
				index = 1;
			}
			else if (bottomQuadrant)
			{
				index = 2;
			}
		}
		// Object can completely fit within the right quadrants
		else if (rect.xStart > verticalMidpoint)
		{
			if (topQuadrant)
			{
				index = 0;
			}
			else if (bottomQuadrant)
			{
				index = 3;
			}
		}
		
		return index;
	}
	
	void Insert(MovableObjectPtr object)
	{
		if (_nodes[0] != nullptr)
		{
			int index = GetIndex(object);
			
			if (index != -1)
			{
				_nodes[index]->Insert(object);
				return;
			}
		}
		
		_objects.push_back(object);
		
		if (_objects.size() > MAX_OBJECTS && _level < MAX_LEVELS)
		{
			if (_nodes[0] == nullptr)
			{
				Split();
			}
			
			int i = 0;
			auto iter = _objects.begin();
			
			while (i < _objects.size() && iter != _objects.end())
			{
				const int index = GetIndex(*iter);
				
				if (index != -1)
				{
					_nodes[index]->Insert(*_objects.erase(iter));
				}
				else
				{
					i++;
					iter++;
				}
			}
		}
	}
	
	std::list<MovableObjectPtr> Retrieve(std::list<MovableObjectPtr>& returnObjects, const MovableObjectPtr& object)
	{
		int index = GetIndex(object);
		
		if (index != -1 && _nodes[0] != nullptr)
		{
			_nodes[index]->Retrieve(returnObjects, object);
		}
		
		std::copy(_objects.begin(), _objects.end(), std::back_insert_iterator<std::list<MovableObjectPtr>>(returnObjects));
		return returnObjects;
	}
	
protected:
	int _level;
	
	std::list<MovableObjectPtr> _objects;
	
	FRect _bounds;
	
	QuadTree* _nodes[4];
};

