#include "stdafx.h"
#include "CollisionUtils.hpp"
#include "MovableObject.hpp"
#include "Bubble.hpp"

namespace CollisionUtils
{
	void ResolveFixedCollision(const MovableObjectPtr& objectPtr, const FPoint& normal, float dt)
	{
		const FPoint& direction = objectPtr->GetDirection();
		const FPoint component = direction.GetDotProduct(normal) * normal;
		const FPoint newDirection = (direction - component) - component;
		objectPtr->SetDirection(newDirection.Normalized());
		objectPtr->UpdatePosition(dt);
	}

	bool DetectStaticBubblesCollision(const FPoint& position1, float radius1, FPoint& moveVector, const FPoint& position2, float radius2)
	{
		const float moveLen = moveVector.Length();

		float distance21 = position2.GetDistanceTo(position1);
		float radiusSum = (radius1 + radius2);
		distance21 -= radiusSum;

		if (moveLen < distance21)
		{
			return false;
		}

		const FPoint normal = moveVector.Normalized();
		const FPoint diffVec = position2 - position1;
		const float dot = normal.GetDotProduct(diffVec);

		if (dot <= 0)
		{
			return false;
		}

		const float diffLen = diffVec.Length();
		const float f = (diffLen * diffLen) - (dot * dot);

		const float radiusSumSquared = math::sqr(radiusSum);
		if (f >= radiusSumSquared)
		{
			return false;
		}

		const float t = radiusSumSquared - f;

		if (t < 0)
		{
			return false;
		}

		const float distance = diffLen - sqrt(t);
		if (moveLen < distance)
		{
			return false;
		}

		moveVector.Normalize();
		moveVector *= distance;
		
		return true;
	}

	bool DetectDynamicBubblesCollision(const BubblePtr& bubble1, const BubblePtr& bubble2, float& time, float dt)
	{
		time = 0.f;
		const FPoint move = bubble1->GetVelocity(dt) - bubble2->GetVelocity(dt);
		FPoint mewMove = move;

		if (!DetectStaticBubblesCollision(bubble1->GetPosition(), bubble1->GetRadius(), mewMove, bubble2->GetPosition(), bubble2->GetRadius()))
		{
			return false;
		}

		time = move.Length() / mewMove.Length();

		if (time < 0.f || time > 1.f)
		{
			return false;
		}

		return true;
	}

	void ResolveBubbleToBubbleCollision(const BubblePtr& bubble1, const BubblePtr& bubble2, float time, float dt)
	{
		const float dv1 = bubble1->GetSpeed() * dt;
		const float dv2 = bubble2->GetSpeed() * dt;

		const FPoint& vel1 = bubble1->GetVelocity(dt);
		const FPoint& vel2 = bubble2->GetVelocity(dt);

		const FPoint& pos1 = bubble1->GetPosition();
		const FPoint& pos2 = bubble2->GetPosition();

		const float d = math::sqrt(math::sqr(pos1.x - pos2.x) + math::sqr(pos1.y - pos2.y));
		const float nx = (pos2.x - pos1.x) / d;
		const float ny = (pos2.y - pos1.y) / d;
		const float p = 2 * (vel1.x * nx + vel1.y * ny - vel2.x * nx - vel2.y * ny) / 2;

		FPoint newVel1, newVel2;
		newVel1.x = vel1.x - p * nx;
		newVel1.y = vel1.y - p * ny;
		newVel2.x = vel2.x + p * nx;
		newVel2.y = vel2.y + p * ny;

		bubble1->SetDirection((newVel1 / dv1).Normalized());
		bubble1->UpdatePosition(dt * (1.f - time));
		bubble1->SetCollided(true);

		bubble2->SetDirection((newVel2 / dv2).Normalized());
		bubble2->UpdatePosition(dt * (1.f - time));
		bubble2->SetCollided(true);
	}
}