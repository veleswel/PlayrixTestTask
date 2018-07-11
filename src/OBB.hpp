#pragma once

/* �����, ����������� ���������� oriented bounding box, ������� ������������ ��� 
����������� ������������ ����� ���������. ��������� ����������� ����� ��������. ������ 
��������� ���� ������������ � ������ OBB, ������ ���������� 4 ������� �����.*/

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
	bool OverlapsOneWay(const OBB& other) const;
	void ComputeAxes();

protected:
	std::array<FPoint, 4> _corner;
	std::array<FPoint, 2> _axis;
	std::array<float, 2> _origin;
};