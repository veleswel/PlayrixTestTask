#include "stdafx.h"
#include "Wall.hpp"
#include "MainSceneWidget.hpp"

const float Wall::Width = 2.f;

Wall::Wall()
	: _start(0.f, 0.f)
	, _end(0.f, 0.f)
	, _normal(0.f, 0.f)
	, _vector(0.f, 0.f)
	, _obb()
{

}

Wall::Wall(const Wall& wall)
{
	operator=(wall);
}

Wall::Wall(const FPoint& start, const FPoint& end)
	: _start(start)
	, _end(end)
	, _normal(0, 0)
	, _vector(0, 0)
	, _obb()
{
	Init();
}

Wall::Wall(float xStart, float yStart, float xEnd, float yEnd)
	: _start(xStart, yStart)
	, _end(xEnd, yEnd)
	, _normal(0.f, 0.f)
	, _vector(0.f, 0.f)
	, _obb()
{
	Init();
}

Wall& Wall::operator = (const Wall& wall)
{
	if (this == &wall)
	{
		return *this;
	}

	_start = wall._start;
	_end = wall._end;
	_normal = wall._normal;
	_vector = wall._vector;
	_obb = wall._obb;

	return *this;
}

bool operator == (const Wall& left, const Wall& right)
{
	return (left._start == right._start) && (left._end == right._end);
}

void Wall::Init()
{
	CalculateVectors();
	CalculateOBB();
}

void Wall::CalculateVectors()
{
	_vector = _end - _start;
	const FPoint v = _vector.Normalized();
	_normal.x = -v.y + 0.f;
	_normal.y = v.x + 0.f;
}

void Wall::CalculateOBB()
{
	float xStart = _start.x;
	float xEnd = _end.x + _normal.x * Width;

	float yStart = _start.y;
	float yEnd = _end.y + _normal.y * Width;

	const float minX = math::min(xStart, xEnd);
	const float maxX = math::max(xStart, xEnd);

	const float width = maxX - minX;

	const float minY = math::min(yStart, yEnd);
	const float maxY = math::max(yStart, yEnd);

	const float height = maxY - minY;

	const FPoint center(minX + width / 2, minY + height / 2);

	_obb = OBB2D(center, width, height, 0.f);
}

const FPoint& Wall::GetNormal() const
{
	return _normal;
}

void Wall::Draw()
{
	if (MainSceneWidget::IsDebugDraw())
	{
		Render::device.SetTexturing(false);

		const auto& corner = _obb.GetCorners();

		Render::BeginColor(Color(0, 255, 255, 255));
		Render::DrawLine(corner[0], corner[1]);
		Render::DrawLine(corner[1], corner[2]);
		Render::DrawLine(corner[2], corner[3]);
		Render::DrawLine(corner[3], corner[0]);
		Render::EndColor();

		Render::device.SetTexturing(true);
	}
}

const OBB2D& Wall::GetOBB() const
{
	return _obb;
}