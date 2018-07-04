#pragma once
#include "GameObject.hpp"

class MovableObject;
typedef boost::intrusive_ptr<MovableObject> MovableObjectPtr;

class OBB2D
{
public:
	math::Vector3 _corner[4];
	math::Vector3 _axis[2];
	float _origin[2];
	
	bool Overlaps1Way(const OBB2D& other) const
	{
		for (int a = 0; a < 2; ++a)
		{
			float t = other._corner[0].DotProduct(_axis[a]);
			
			float tMin = t;
			float tMax = t;
			
			for (int c = 1; c < 4; ++c)
			{
				t = other._corner[c].DotProduct(_axis[a]);
	
				if (t < tMin)
				{
					tMin = t;
				}
				else if (t > tMax)
				{
					tMax = t;
				}
			}
			
			if ((tMin > 1.f + _origin[a]) || (tMax < _origin[a]))
			{
				return false;
			}
		}
		
		return true;
	}
	
	void ComputeAxes()
	{
		_axis[0] = _corner[1] - _corner[0];
		_axis[1] = _corner[3] - _corner[0];
		
		for (int a = 0; a < 2; ++a)
		{
			_axis[a] /= _axis[a].LengthSq();
			_origin[a] = _corner[0].DotProduct(_axis[a]);
		}
	}
	
public:
	OBB2D()
	{
		
	}
	
	OBB2D(const math::Vector3& position, float width, float height, float angle)
	{
		math::Vector3 X(math::cos(angle), math::sin(angle), 0);
		math::Vector3 Y(-math::sin(angle), math::cos(angle), 0);
		
		X *= width / 2;
		Y *= height / 2;
		
		_corner[0] = position - X - Y;
		_corner[1] = position + X - Y;
		_corner[2] = position + X + Y;
		_corner[3] = position - X + Y;
		
		ComputeAxes();
	}
	
	OBB2D(const OBB2D& other)
	{
		_corner[0] = other._corner[0];
		_corner[1] = other._corner[1];
		_corner[2] = other._corner[2];
		_corner[3] = other._corner[3];
		
		_axis[0] = other._axis[0];
		_axis[1] = other._axis[1];
		
		_origin[0] = other._origin[0];
		_origin[1] = other._origin[1];
	}
	
	OBB2D& operator = (const OBB2D& other)
	{
		_corner[0] = other._corner[0];
		_corner[1] = other._corner[1];
		_corner[2] = other._corner[2];
		_corner[3] = other._corner[3];
		
		_axis[0] = other._axis[0];
		_axis[1] = other._axis[1];
		
		_origin[0] = other._origin[0];
		_origin[1] = other._origin[1];
		
		return *this;
	}
	
	bool Overlaps(const OBB2D& other) const
	{
		return Overlaps1Way(other) && other.Overlaps1Way(*this);
	}
};

class MovableObject: public GameObject
{
public:
	virtual ~MovableObject();

protected:
	MovableObject();
	
public:
	virtual void Draw() override;
	virtual void Update(float dt) override;
	
	void SetVelocity(const math::Vector3& velocity);
	const math::Vector3& GetVelocity() const;

	OBB2D GetOBB() const;
	
protected:
	void Init(const std::string& textureName, float speed);
	void UpdatePosition(float dt);
	
protected:
	float _speed;
	math::Vector3 _velocity;
};
