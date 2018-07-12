#include "stdafx.h"
#include "CollisionUtils.hpp"
#include "MovableObject.hpp"
#include "Bubble.hpp"

namespace CollisionUtils
{
	// Разрешает столкновение движущегося объекта и статического объекта (стены)
	// В качестве параметров принимает объект, вектор нормали стены и отрезок времени

	void ResolveFixedCollision(const MovableObjectPtr& objectPtr, const FPoint& normal, float dt)
	{
		const FPoint& direction = objectPtr->GetDirection();
		const FPoint component = direction.GetDotProduct(normal) * normal;
		const FPoint newDirection = (direction - component) - component;
		objectPtr->SetDirection(newDirection.Normalized());

		// После рассчета нового вектора направления двигаем объект в новую позицию
		objectPtr->UpdatePosition(dt);
	}


	// Метод, который определяет возможность столкновения движущегося шара и статического шара. В качестве параметров принимает
	// позицию, радиус и вектор скорости первого шара и позицию и радиус второго шара. Возвращает true, если столкновение возможно. Так же
	// меняет вектор скорости первого шара в то состояние, в котором он будет после столкновения. Сам алгоритм и рассчеты были 
	// взяты здесь https://www.gamasutra.com/view/feature/131424/pool_hall_lessons_fast_accurate_.php и адаптированы для собственных нужд

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


	// Метод, который определяет возможность столкновения двух движущихся шаров. Использует предыдущий метод и указанный алгоритм.
	// Возвращает true, если столкновение возможно, и меняет переменную time, которая будет содержать время до столкновения

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

	// Метод, разрешающий столкновение двух шаров. Принимает в качестве параметров оба шара, время до столкновение и время апдейта текущего фрейма.
	// Новые векторы направления рассчитываются по формуле идеально эластичного столкновения, когда масса объектов, их материал и трение не имеют значения

	void ResolveBubbleToBubbleCollision(const BubblePtr& bubble1, const BubblePtr& bubble2, float time, float dt)
	{
		const FPoint& vel1 = bubble1->GetVelocity(dt);
		const FPoint& vel2 = bubble2->GetVelocity(dt);

		const FPoint& pos1 = bubble1->GetPosition();
		const FPoint& pos2 = bubble2->GetPosition();

		// Рассчитываем прирост скорости для каждого шара в текущий момент времени
		const float dv1 = bubble1->GetSpeed() * dt;
		const float dv2 = bubble2->GetSpeed() * dt;

		// Рассчитываем новые векторы скоростей
		const float d = math::sqrt(math::sqr(pos1.x - pos2.x) + math::sqr(pos1.y - pos2.y));
		const float nx = (pos2.x - pos1.x) / d;
		const float ny = (pos2.y - pos1.y) / d;
		const float p = 2 * (vel1.x * nx + vel1.y * ny - vel2.x * nx - vel2.y * ny) / 2;

		const FPoint newVel1(vel1.x - p * nx, vel1.y - p * ny);
		const FPoint newVel2(vel2.x + p * nx, vel2.y + p * ny);

		// Устанавливаем векторы направления для шаров на основе новых векторов скоростей
		// Перед установкой выполняем нормализацию вектора направления, и потом сдвигаем объект в новую позицию

		bubble1->SetDirection((newVel1 / dv1).Normalized());
		bubble1->UpdatePosition(dt * (1.f - time));
		bubble1->SetCollided(true);

		bubble2->SetDirection((newVel2 / dv2).Normalized());
		bubble2->UpdatePosition(dt * (1.f - time));
		bubble2->SetCollided(true);
	}
}