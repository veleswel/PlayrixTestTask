//
//  MainSceneWidget.cpp
//  Test
//
//  Created by Alexey Vlasenko on 6/29/18.
//

#include "stdafx.h"
#include "MainSceneWidget.hpp"

const float MainSceneWidget::Speed = 50.f;

MainSceneWidget::MainSceneWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _timer(0.f)
	, _cannon(nullptr)
	, _projectile(nullptr)
	, _projPosition(FPoint(0.f, 0.f))
	, _angle(0.f)
	, _isProjectileLaunched(false)
	, _startPosition(FPoint(Render::device.Width() / 2, 0.f))
{
	Init();
}

void MainSceneWidget::Init()
{
	_cannon = Cannon::create();
	_cannon->SetPosition(_startPosition);
	_cannon->SetAnchorPoint(FPoint(.5f, 0.f));
	_cannon->SetRotationAngle(0.f);
}

void MainSceneWidget::Draw()
{
	IPoint mouse_pos = Core::mainInput.GetMousePos();
	
	float width = Render::device.Width();
	float height = Render::device.Height();
	
	Render::device.SetTexturing(false);
	Render::BeginColor(Color(106, 126, 160, 255));
	Render::DrawRect(0, 0, width, height);
	Render::EndColor();
	Render::device.SetTexturing(true);
	
	_cannon->Draw();
	
	if (_isProjectileLaunched && _projectile)
	{
		Render::device.PushMatrix();
		Render::device.MatrixTranslate(_projPosition);
		auto projRect = _projectile->getBitmapRect();
		Render::device.MatrixTranslate(-projRect.Width() / 2, -projRect.Height() / 2, 0.f);
		_projectile->Draw();
		Render::device.PopMatrix();
	}
	
	Render::device.SetTexturing(false);
	Render::BeginColor(Color(255, 128, 0, 255));
	Render::DrawRect(924, 0, 100, 100);
	Render::EndColor();
	Render::device.SetTexturing(true);
	
	_effCont.Draw();
	
	Render::BindFont("arial");
	Render::PrintString(924 + 100 / 2, 50, utils::lexical_cast(mouse_pos.x) + ", " + utils::lexical_cast(mouse_pos.y), 1.f, CenterAlign);
	
	Render::PrintString(924 + 100 / 2, 35, utils::lexical_cast(_angle), 1.f, CenterAlign);
}

void MainSceneWidget::Update(float dt)
{
	UpdateCannon(dt);
	UpdateBubbles(dt);
	UpdateProjectiles(dt);

	if (_isProjectileLaunched)
	{
		_projPosition.x += math::cos(_angle) * (Speed * dt * 10);
		_projPosition.y += math::sin(_angle) * (Speed * dt * 10);
		
		FRect screenRect(0.f, Render::device.Width(), 0.f, Render::device.Height());
		if (!screenRect.Contains(_projPosition))
		{
			DestroyProjectile();
		}
	}
	
	_effCont.Update(dt);
	
	_timer += dt * 2;
	
	while (_timer > 2 * math::PI)
	{
		_timer -= 2 * math::PI;
	}
}

bool MainSceneWidget::MouseDown(const IPoint &mouse_pos)
{
	LaunchProjectile(mouse_pos);

	float angle = _cannon->GetRotationAngle();

	if (Core::mainInput.GetMouseRightButton())
	{
		angle -= 15;
	}
	else
	{
		angle += 15;
	}

	while (angle > 360)
	{
		angle -= 360;
	}

	_cannon->SetRotationAngle(angle);

	return false;
}

void MainSceneWidget::MouseMove(const IPoint &mouse_pos)
{

}

void MainSceneWidget::MouseUp(const IPoint &mouse_pos)
{
	
}

void MainSceneWidget::AcceptMessage(const Message& message)
{
	//
	// Виджету могут посылаться сообщения с параметрами.
	//
	
	const std::string& publisher = message.getPublisher();
	const std::string& data = message.getData();
}

void MainSceneWidget::KeyPressed(int keyCode)
{
	//
	// keyCode - виртуальный код клавиши.
	// В качестве значений для проверки нужно использовать константы VK_.
	//
	
	if (keyCode == VK_A)
	{
		// Реакция на нажатие кнопки A
	}
}

void MainSceneWidget::LaunchProjectile(const IPoint& position)
{
	FPoint mousePosition((float)position.x, (float)position.y);
	
	float slope = (mousePosition.y - _startPosition.y) / (mousePosition.x - _startPosition.x);
	_angle = atan(slope);
	if (_angle < 0)
	{
		_angle += math::PI;
	}
	_projPosition = _startPosition;
	_projectile = Core::resourceManager.Get<Render::Texture>("Star");
	_isProjectileLaunched = true;
}

void MainSceneWidget::DestroyProjectile()
{
	_isProjectileLaunched = false;
	_projectile = nullptr;
//	_angle = 0.f;
}

void MainSceneWidget::UpdateBubbles(float dt)
{
}

void MainSceneWidget::UpdateProjectiles(float dt)
{
}

void MainSceneWidget::UpdateCannon(float dt)
{
	_cannon->Update(dt);

	FPoint mousePosition = static_cast<FPoint>(Core::mainInput.GetMousePos());

	float slope = (mousePosition.y - _startPosition.y) / (mousePosition.x - _startPosition.x);
	float angle = atan(slope);
	if (angle < 0)
	{
		angle += math::PI;
	}

	angle = (angle * 180) / math::PI;

	//_cannon->SetRotationAngle(angle);
}