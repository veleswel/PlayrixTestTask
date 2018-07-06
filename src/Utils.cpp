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
	
	const math::Vector3 NormalVector(const math::Vector3& vector)
	{
		return math::Vector3(-vector.y, vector.x, 0.f);
	}
	
	float Component(const math::Vector3& vector, const math::Vector3 &directionVector)
	{
		const float alpha = math::atan(directionVector.y, directionVector.x);
		const float theta = math::atan(vector.y, vector.x);
		const float len = vector.Length();
		
		return len * math::cos(theta - alpha);
	}
	
	const math::Vector3 ComponentVector(const math::Vector3& vector, const math::Vector3& directionVector)
	{
		const math::Vector3 v(directionVector.Normalized());
		return Component(vector, directionVector) * v;
	}
	
	void ResolveFixedCollision(const MovableObjectPtr& objectPtr, const math::Vector3& normal)
	{
		const math::Vector3& velocity = objectPtr->GetVelocity();
		const math::Vector3 component = ComponentVector(velocity, normal);
		objectPtr->SetVelocity(velocity - 2 * component);
	}
	
	bool CircleCircleCollision(const BubblePtr& bubble1, const BubblePtr& bubble2, std::tuple<float, math::Vector3>& collision)
	{
		const math::Vector3 bubblePos1(bubble1->GetPosition().x, bubble1->GetPosition().y, 0);
		const math::Vector3 bubblePos2(bubble2->GetPosition().x, bubble2->GetPosition().y, 0);
		
		const math::Vector3 w =  bubblePos1 - bubblePos2;
		const math::Vector3 n = w.Normalized();
		
		float r = bubble1->GetRadius() + bubble2->GetRadius();
		float ww = w.DotProduct(w);
		
		if (ww < r * r)
		{
			return false;
		}
		
		const math::Vector3 v = bubble1->GetVelocity() - bubble2->GetVelocity();
		
		float a = v.DotProduct(v);
		float b = w.DotProduct(v);
		
		float c = ww-r*r;
		float root = b*b-a*c;
		
		if (root < 0)
		{
			return false;
		}
		
		float t = (-b - math::sqrt(root)) / a;
		if (t > 1.f || t < 0.f)
		{
			return false;
		}
		
		std::get<0>(collision) = t;
		std::get<1>(collision) = n;
		
		return true;
	}

	void ResolveCollisionEqualMass(const BubblePtr& bubble1, const BubblePtr& bubble2)
	{
		const math::Vector3 pos1(bubble1->GetPosition().x, bubble1->GetPosition().y, 0);
		const math::Vector3 pos2(bubble2->GetPosition().x, bubble2->GetPosition().y, 0);
		
		const math::Vector3& v1 = bubble1->GetVelocity();
		const math::Vector3& v2 = bubble2->GetVelocity();
		
		auto u = v1 - v2;
		auto n = (pos1 - pos2).Normalized();
		
		auto un = ComponentVector(u, n);
		
		auto ut = u - un;
		
		const math::Vector3 newV1(ut + v2);
		const math::Vector3 newV2(un + v2);
		
		bubble1->SetVelocity(newV1);
		bubble2->SetVelocity(newV2);
	}
	
	void CheckCollision(float dt, const std::vector<BubblePtr>& movableObjects)
	{
		float mn = 2.f;
		BubblePtr ob1 = 0;
		BubblePtr ob2 = 0;
		
		for (int i = 0; i < movableObjects.size(); ++i)
		{
			BubblePtr obj1 = movableObjects[i];
			for (int j = i + 1; j < movableObjects.size(); ++j)
			{
				BubblePtr obj2 = movableObjects[j];
				std::tuple<float, math::Vector3> c;
				if (!CircleCircleCollision(obj1, obj2, c))
				{
					continue;
				}
				
				float tm = std::get<0>(c);
				float m = math::min(mn, tm);
				
				if (m < mn)
				{
					mn = m;
					//auto n = std::get<1>(c); // the normal vector
					ob1 = obj1;
					ob2 = obj2;
				}
			}
		}
		
		float tmove = 0.f;
		
		if (mn == 2)
		{
			tmove = 1.f;
		}
		else
		{
			tmove = mn * dt;
		}
		
		for (const MovableObjectPtr& object: movableObjects)
		{
			object->UpdatePosition(tmove);
		}
		
		if (mn == 2)
		{
			return;
		}
		
		ResolveCollisionEqualMass(ob1, ob2);
		
		CheckCollision(dt * (1 - mn), movableObjects);
	}
}

void collision2D(float r1, float r2,
				 float& x1, float& y1,
				 float& x2, float& y2,
				 float& vx1, float& vy1,
				 float& vx2, float& vy2,
				 int& error)
{
	float  r12, m21, d, gammav, gammaxy, dgamma, dr, dc, sqs, t, dvx2, a, x21, y21, vx21, vy21, pi2, vx_cm, vy_cm;
	float R = 1;
	float m1 = 1;
	float m2 = 1;
	// ***initialize some variables ****
	pi2 = 2 * acos(-1.0E0);
	error = 0;
	r12 = r1+r2;
	
	m21 = 1;
	x21 = x2-x1;
	y21 = y2-y1;
	vx21 = vx2-vx1;
	vy21 = vy2-vy1;
	
	vx_cm = (m1 * vx1 + m2 * vx2) / (m1+m2);
	vy_cm = (m1 * vy1 + m2 * vy2) / (m1+m2);
	
	//     ****  return old positions and velocities if relative velocity =0 ****
	if (vx21==0 && vy21==0)
	{
		error = 1;
		return;
	}
	
	
	//     *** calculate relative velocity angle
	gammav=atan2(-vy21,-vx21);
	
	
	//******** this block only if initial positions are given *********
	d=sqrt(x21*x21 +y21*y21);
	
	//     **** return if distance between balls smaller than sum of radii ***
	if (d<r12) {error=2; return;}
	
	//     *** calculate relative position angle and normalized impact parameter ***
	gammaxy=atan2(y21,x21);
	dgamma=gammaxy-gammav;
	
	if (dgamma>pi2) {dgamma=dgamma-pi2;}
	else if (dgamma<-pi2) {dgamma=dgamma+pi2;}
	
	dr=d*sin(dgamma)/r12;
	
	//     **** return old positions and velocities if balls do not collide ***
	if ((fabs(dgamma) > pi2 / 4 && fabs(dgamma) < 0.75 * pi2) || fabs(dr) > 1)
	{
		error=1;
		return;
	}
	
	
	//     **** calculate impact angle if balls do collide ***
	float alpha=asin(dr);
	
	//     **** calculate time to collision ***
	dc=d*cos(dgamma);
	if (dc>0) {sqs=1.0;} else {sqs=-1.0;}
	t=(dc-sqs*r12*sqrt(1-dr*dr))/sqrt(vx21*vx21+ vy21*vy21);
	//    **** update positions ***
	x1=x1+vx1*t;
	y1=y1+vy1*t;
	x2=x2+vx2*t;
	y2=y2+vy2*t;
	
	a=tan(gammav + alpha);
	
	dvx2 = -2 * (vx21 + a * vy21) / ((1+a*a)*(1+m21));
	
	vx2 = vx2 + dvx2;
	vy2 = vy2 + a * dvx2;
	vx1 = vx1 - m21 * dvx2;
	vy1= vy1 - a * m21 * dvx2;
	
	//     ***  velocity correction for inelastic collisions ***
	
	vx1=(vx1-vx_cm)*R + vx_cm;
	vy1=(vy1-vy_cm)*R + vy_cm;
	vx2=(vx2-vx_cm)*R + vx_cm;
	vy2=(vy2-vy_cm)*R + vy_cm;
	
	return;
}
