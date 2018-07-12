#pragma once

// Класс для реализации функционала OBB(oriented bounding box)
// Отличается от AABB тем, что может быть произвольно развернут относительно осей координат
// Используется для обнаружения столкновений между объектами
// Алгоритм был взят отсюда http://www.flipcode.com/archives/2D_OBB_Intersection.shtml
// и адаптирован под свои нужды

class OBB
{
public:
	OBB();
	OBB(const FPoint& center, float width, float height, float angle);
	OBB(const OBB& other);
	OBB& operator = (const OBB& other);

public:
	bool Overlaps(const OBB& other) const;
	const std::array<FPoint, 4>& GetCorners() const;

protected:
	bool OverlapsOther(const OBB& other) const;
	void ComputeAxes();

protected:
	std::array<FPoint, 4> _corner;
	std::array<FPoint, 2> _axis;
	std::array<float, 2> _origin;
};
