#pragma once
#include "OBB.hpp"

// Класс, который реализует невидимую стену.
// Объекты этого класса размещаются по сторонам экрана и
// используются для обнаружении столкновений объектов с экраном.

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
	// Возвращает вектор нормали стены
	const FPoint& GetNormal() const;
	
	// Возвращает OBB
	const OBB& GetOBB() const;

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

	OBB _obb;
};

typedef std::unique_ptr<Wall> WallPtr;
