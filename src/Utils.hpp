#pragma once
#include "MovableObject.hpp"
#include "Bubble.hpp"

namespace Utils
{
	float DegreeToRadian(float degree);
	float RadianToDegree(float rad);

	void ResolveFixedCollision(const MovableObjectPtr& objectPtr, const math::Vector3& normal);

	bool Collision2D(
		float r1, float r2,
		const FPoint& p1,
		const FPoint& p2,
		float& vx1, float& vy1,
		float& vx2, float& vy2,
		float& time);

	bool SweptAABB(const FRect& b1, const FRect& b2, const math::Vector3& v1, float& time);
}
