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
		math::Vector3& v1,
		math::Vector3& v2,
		float& time);

	bool SweptAABB(const FRect& b1, const FRect& b2, const math::Vector3& v1, float& time);

	std::pair<bool, math::Vector3> some(math::Vector3 A, float rA, math::Vector3 B, float rB, math::Vector3 movevec);

	std::pair<bool, float> some1(math::Vector3 A, float rA, math::Vector3 B, float rB, math::Vector3 movevec1, math::Vector3 movevec2);
}
