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
		objectPtr->SetAcceleration(100.f);
	}

	bool Collision2D(
		float r1, float r2,
		const FPoint& p1,
		const FPoint& p2,
		math::Vector3& v1,
		math::Vector3& v2,
		float& time)
	{
		float  r12, d, gammav, gammaxy, dgamma, dr, dc, sqs, dvx2, a, x21, y21, vx21, vy21, pi2;

		// ***initialize some variables ****
		pi2 = 2 * math::PI;
		r12 = r1 + r2;
	
		x21 = p2.x - p1.x;
		y21 = p2.y - p1.y;
		vx21 = v2.x - v1.x;
		vy21 = v2.y - v1.y;
	
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
	
		v2.x = v2.x + dvx2;
		v2.y = v2.y + a * dvx2;
		v1.x = v1.x - dvx2;
		v1.y = v1.y - a * dvx2;
	
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

	std::pair<bool, math::Vector3> some(math::Vector3 A, float rA, math::Vector3 B, float rB, math::Vector3 movevec)
	{
		// Early Escape test: if the length of the movevec is less
		// than distance between the centers of these circles minus 
		// their radii, there's no way they can hit. 

		std::pair<bool, math::Vector3> res;
		res.first = false;

		double dist = B.Distance(A);
		double sumRadii = (rA + rB);
		dist -= sumRadii;
		if (movevec.Length() < dist) 
		{
			return res;
		}

		// Normalize the movevec
		math::Vector3 N = movevec.Normalized();

		// Find C, the vector from the center of the moving 
		// circle A to the center of B
		math::Vector3 C = B - A;

		// D = N . C = ||C|| * cos(angle between N and C)
		double D = N.DotProduct(C);

		// Another early escape: Make sure that A is moving 
		// towards B! If the dot product between the movevec and 
		// B.center - A.center is less that or equal to 0, 
		// A isn't isn't moving towards B
		if (D <= 0) 
		{
			return res;
		}

		// Find the length of the vector C
		double lengthC = C.Length();

		double F = (lengthC * lengthC) - (D * D);

		// Escape test: if the closest that A will get to B 
		// is more than the sum of their radii, there's no 
		// way they are going collide
		double sumRadiiSquared = sumRadii * sumRadii;
		if (F >= sumRadiiSquared) 
		{
			return res;
		}

		// We now have F and sumRadii, two sides of a right triangle. 
		// Use these to find the third side, sqrt(T)
		double T = sumRadiiSquared - F;

		// If there is no such right triangle with sides length of 
		// sumRadii and sqrt(f), T will probably be less than 0. 
		// Better to check now than perform a square root of a 
		// negative number. 
		if (T < 0) 
		{
			return res;
		}

		// Therefore the distance the circle has to travel along 
		// movevec is D - sqrt(T)
		double distance = D - sqrt(T);

		// Get the magnitude of the movement vector
		double mag = movevec.Length();

		// Finally, make sure that the distance A has to move 
		// to touch B is not greater than the magnitude of the 
		// movement vector. 
		if (mag < distance) 
		{
			return res;
		}

		// Set the length of the movevec so that the circles will just touch
		movevec.Normalize();
		movevec *= distance;
		res.first = true;
		res.second = movevec;
		return res; 
	}

	std::pair<bool, float> some1(math::Vector3 A, float rA, math::Vector3 B, float rB, math::Vector3 movevec1, math::Vector3 movevec2)
	{
		math::Vector3 V1 = movevec1 - movevec2;
		auto res = some(A, rA, B, rB, V1);
		std::pair<bool, float> ret;
		ret.first = false;

		if (!res.first)
		{
			return ret;
		}

		float value = V1.Length() / res.second.Length();

		if (value < 0.f || value > 1.f)
		{
			return ret;
		}

		Log::Debug(std::to_string(value));
		ret.first = true;
		ret.second = value;
		return ret;
	}
}
