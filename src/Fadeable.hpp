#pragma once
#include "Utils.hpp"

class Fadeable
{
protected:
	Fadeable();
	virtual ~Fadeable();

protected:
	void BeginDrawingFading();
	void EndDrawingFading();
	
	void UpdateFading(float dt);

protected:
	float _fadingUpdateRate;
	float _fading;
	float _fadingBound;

	Render::ShaderProgramPtr _fade;
};

