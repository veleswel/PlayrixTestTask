#pragma once

class MovableObject;
typedef boost::intrusive_ptr<MovableObject> MovableObjectPtr;

class Bubble;
typedef boost::intrusive_ptr<Bubble> BubblePtr;

// Набор функций для обнаружения и разрешения столкновений между объектами

namespace CollisionUtils
{
	enum class EColliderType
	{
		EProjectile,
		EBubble
	};

	void ResolveFixedCollision(const MovableObjectPtr& objectPtr, const FPoint& normal, float dt);

	bool DetectStaticBubblesCollision(const FPoint& position1, float radius1, FPoint& moveVector, const FPoint& position2, float radius2);

	bool DetectDynamicBubblesCollision(const BubblePtr& bubble1, const BubblePtr& bubble2, float& time, float dt);

	void ResolveBubbleToBubbleCollision(const BubblePtr& bubble1, const BubblePtr& bubble2, float time, float dt);
}