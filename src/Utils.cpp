#include "stdafx.h"
#include "Utils.hpp"

namespace Utils
{
	float DegreeToRadian(float degree)
	{
		return (degree * math::PI) / 180;
	}

	float RadianToDegree(float rad)
	{
		return (rad * 180) / math::PI;
	}
	
	void ResolveFixedCollision(const MovableObjectPtr& objectPtr, const math::Vector3& normal)
	{
		const math::Vector3& velocity = objectPtr->GetVelocity();
		const math::Vector3 u = velocity.DotProduct(normal) * normal;
		const math::Vector3 w = velocity - u;
		const math::Vector3 v1 = w - u;
		objectPtr->SetVelocity(v1.Normalized());
	}

	bool Collision2D(
		float r1, float r2,
		const FPoint& p1,
		const FPoint& p2,
		float& vx1, float& vy1,
		float& vx2, float& vy2,
		float& time)
	{
		float  r12, d, gammav, gammaxy, dgamma, dr, dc, sqs, dvx2, a, x21, y21, vx21, vy21, pi2;

		// ***initialize some variables ****
		pi2 = 2 * math::PI;
		r12 = r1 + r2;
	
		x21 = p2.x - p1.x;
		y21 = p2.y - p1.y;
		vx21 = vx2 - vx1;
		vy21 = vy2 - vy1;
	
		//     ****  return old positions and velocities if relative velocity =0 ****
		if (vx21==0 && vy21==0)
		{
			return false;
		}

		//     *** calculate relative velocity angle
		gammav = atan2(-vy21, -vx21);
	
	
		//******** this block only if initial positions are given *********
		d = sqrt(x21 * x21 + y21 * y21);
	
		//     **** return if distance between balls smaller than sum of radii ***
		if (d < r12) 
		{
			return false;
		}
	
		//     *** calculate relative position angle and normalized impact parameter ***
		gammaxy = atan2(y21, x21);
		dgamma = gammaxy - gammav;
	
		if (dgamma>pi2) 
		{
			dgamma = dgamma - pi2;
		}
		else if (dgamma < -pi2) 
		{
			dgamma = dgamma + pi2;
		}
	
		dr = d * sin(dgamma) / r12;
	
		//     **** return old positions and velocities if balls do not collide ***
		if ((fabs(dgamma) > pi2 / 4.f && fabs(dgamma) < .75f * pi2) || fabs(dr) > 1.f)
		{
			return false;
		}
	
		//     **** calculate impact angle if balls do collide ***
		float alpha = asin(dr);
	
		//     **** calculate time to collision ***
		dc = d * cos(dgamma);
		if (dc > 0) 
		{
			sqs = 1.f;
		} 
		else
		{
			sqs= -1.f;
		}

		time = (dc - sqs * r12 * sqrt(1.f - dr * dr)) / sqrt(vx21 * vx21 + vy21 * vy21);

		a = tan(gammav + alpha);
	
		dvx2 = -2 * (vx21 + a * vy21) / ((1 + a * a) * 2);
	
		vx2 = vx2 + dvx2;
		vy2 = vy2 + a * dvx2;
		vx1 = vx1 - dvx2;
		vy1 = vy1 - a * dvx2;
	
		return true;
	}

	bool SweptAABB(const FRect& b1, const FRect& b2, const math::Vector3& v1, float& time)
	{
		float xInvEntry, yInvEntry;
		float xInvExit, yInvExit;

		// find the distance between the objects on the near and far sides for both x and y
		if (v1.x > 0.f)
		{
			xInvEntry = b2.xStart - (b1.xStart + b1.Width());
			xInvExit = (b2.xStart + b2.Width()) - b1.xStart;
		}
		else
		{
			xInvEntry = (b2.xStart + b2.Width()) - b1.xStart;
			xInvExit = b2.xStart - (b1.xStart + b1.Width());
		}

		if (v1.y > 0.0f)
		{
			yInvEntry = b2.yStart - (b1.yStart + b1.Height());
			yInvExit = (b2.yStart + b2.Height()) - b1.yStart;
		}
		else
		{
			yInvEntry = (b2.yStart + b2.Height()) - b1.yStart;
			yInvExit = b2.yStart - (b1.yStart + b1.Height());
		}

		// find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)
		float xEntry, yEntry;
		float xExit, yExit;
		if (v1.x == 0.f)
		{
			xEntry = -std::numeric_limits<float>::infinity();
			xExit = std::numeric_limits<float>::infinity();
		}
		else
		{
			xEntry = xInvEntry / v1.x;
			xExit = xInvExit / v1.x;
		}

		if (v1.y == 0.f)
		{
			yEntry = -std::numeric_limits<float>::infinity();
			yExit = std::numeric_limits<float>::infinity();
		}
		else
		{
			yEntry = yInvEntry / v1.y;
			yExit = yInvExit / v1.y;
		}

		// find the earliest/latest times of collision
		float entryTime = std::max(xEntry, yEntry);
		float exitTime = std::min(xExit, yExit);

		// if there was no collision
		if (entryTime > exitTime || xEntry < 0.f && yEntry < 0.f || xEntry > 1.f || yEntry > 1.f)
		{
			time = 1.f;
			return false;
		}
		else // if there was a collision
		{
			time = entryTime;
			return true;
		}
	}
}
