#pragma once
class OBB2D
{
public:
	OBB2D();
	OBB2D(const math::Vector3& center, float width, float height, float angle);
	OBB2D(const OBB2D& other);
	OBB2D& operator = (const OBB2D& other);

	bool Overlaps(const OBB2D& other) const;

	const std::array<math::Vector3, 4>& GetCorners() const;

protected:
	bool Overlaps1Way(const OBB2D& other) const;
	void ComputeAxes();

protected:
	std::array<math::Vector3, 4> _corner;
	std::array<math::Vector3, 2> _axis;
	std::array<float, 2> _origin;
};