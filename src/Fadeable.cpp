#include "stdafx.h"
#include "Fadeable.hpp"

Fadeable::Fadeable()
	: _fade(nullptr)
	, _fading(1.f)
	, _fadingUpdateRate(.5f)
	, _fadingBound(1.f)
{
	_fade = Core::resourceManager.Get<Render::ShaderProgram>("fade");
}

Fadeable::~Fadeable()
{
	
}

void Fadeable::BeginDrawingFading()
{
	_fade->Bind();
	_fade->SetUniform("u_modelview", math::Matrix4::Identity);
	_fade->SetUniform("sampler", 0);
	_fade->SetUniform("multiplier", _fading);
}

void Fadeable::EndDrawingFading()
{
	_fade->Unbind();
}

void Fadeable::UpdateFading(float dt)
{
	if (math::IsEqualFloat(_fading, _fadingBound, 0.001f))
	{
		return;
	}

	if (_fading > _fadingBound)
	{
		_fading -= dt * _fadingUpdateRate;
	}
	else if (_fading < _fadingBound)
	{
		_fading += dt * _fadingUpdateRate;
	}
}