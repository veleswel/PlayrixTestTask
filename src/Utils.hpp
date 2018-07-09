#pragma once
#include "MovableObject.hpp"
#include "Bubble.hpp"

namespace Utils
{
	enum class EGameWidgetState
	{
		EMenu, EPlaying, EPause, EFinish
	};

	float DegreeToRadian(float degree);
	float RadianToDegree(float rad);
}
