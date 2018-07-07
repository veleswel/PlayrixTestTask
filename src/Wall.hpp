#pragma once
#include "OBB.hpp"

class Wall
{
public:
	Wall();

	Wall(const Wall& wall);

	Wall(const FPoint& start, const FPoint& end);

	Wall(float xStart, float yStart, float xEnd, float yEnd);

	Wall& operator = (const Wall& wall);

	friend bool operator == (const Wall& left, const Wall& right);

public:
	const FPoint& GetNormal() const;

	void Draw();

	const OBB2D& GetOBB() const;

protected:
	void Init();
	void CalculateVectors();
	void CalculateOBB();

protected:
	static const float Width;

protected:
	FPoint _start;
	FPoint _end;

	FPoint _vector;
	FPoint _normal;

	OBB2D _obb;
};

typedef std::unique_ptr<Wall> WallPtr;