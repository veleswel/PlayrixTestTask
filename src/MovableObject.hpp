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
	
	bool overlaps1Way(const OBB2D& other) const
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
			
			// We have to subtract off the origin
			
			// See if [tMin, tMax] intersects [0, 1]
			if ((tMin > 1.f + _origin[a]) || (tMax < _origin[a]))
			{
				// There was no intersection along this dimension;
				// the boxes cannot possibly overlap.
				return false;
			}
		}
		
		// There was no dimension along which there is no intersection.
		// Therefore the boxes overlap.
		return true;
	}
	
	/** Updates the axes after the corners move.  Assumes the
	 corners actually form a rectangle. */
	void computeAxes()
	{
		_axis[0] = _corner[1] - _corner[0];
		_axis[1] = _corner[3] - _corner[0];
		
		// Make the length of each axis 1/edge length so we know any
		// dot product must be less than 1 to fall within the edge.
		
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
	
	OBB2D(const math::Vector3& center, float width, float height, float angle)
	{
		math::Vector3 X(math::cos(angle), math::sin(angle), 0);
		math::Vector3 Y(-math::sin(angle), math::cos(angle), 0);
		
		X *= width / 2;
		Y *= height / 2;
		
		_corner[0] = center - X - Y;
		_corner[1] = center + X - Y;
		_corner[2] = center + X + Y;
		_corner[3] = center - X + Y;
		
		computeAxes();
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
	
	/** Returns true if the intersection of the boxes is non-empty. */
	bool overlaps(const OBB2D& other) const
	{
		return overlaps1Way(other) && other.overlaps1Way(*this);
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

	void UpdatePosition(float dt);

	void SetVelocity(const math::Vector3& velocity);
	const math::Vector3& GetVelocity() const;

	float GetSpeed() const;
	void Stop();

	OBB2D GetOBB() const;
	
protected:
	void Init(const std::string& textureName, float speed);
	
protected:
	float _speed;
	math::Vector3 _velocity;
};
