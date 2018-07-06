#pragma once
#include "MovableObject.hpp"
#include "Bubble.hpp"

namespace Utils
{
	float DegreeToRadian(float degree);
	float RadianToDegree(float rad);
	
	const math::Vector3 NormalVector(const math::Vector3& vector);
	float Component(const math::Vector3& vector, const math::Vector3 &directionVector);
	const math::Vector3 ComponentVector(const math::Vector3& vector, const math::Vector3& directionVector);
	void ResolveFixedCollision(const MovableObjectPtr& objectPtr, const math::Vector3& normal);
	
	bool CircleCircleCollision(const BubblePtr& bubble1, const BubblePtr& bubble2, std::tuple<float, math::Vector3>& collision);

	void ResolveCollisionEqualMass(const BubblePtr& bubble1, const BubblePtr& bubble2);
	
	void CheckCollision(float dt, const std::vector<BubblePtr>& movableObjects);
}

void collision2D(float r1, float r2,
				 float& x1, float& y1,
				 float& x2, float& y2,
				 float& vx1, float& vy1,
				 float& vx2, float& vy2,
				 int& error);
