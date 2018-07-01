#include "stdafx.h"
#include "MainSceneWidget.hpp"

const float MainSceneWidget::Speed = 50.f;

MainSceneWidget::MainSceneWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _timer(0.f)
	, _cannon(nullptr)
	, _startPosition(FPoint(Render::device.Width() / 2, 0.f))
{
	Init();
}

void MainSceneWidget::Init()
{
	_cannon = Cannon::create();
	_cannon->SetPosition(_startPosition);
	_cannon->SetAnchorPoint(FPoint(0.f, .5f));
	_cannon->SetRotationAngle(90.f);
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
	
	DrawBubbles();
	DrawProjectiles();

	_cannon->Draw();
	
	Render::device.SetTexturing(false);
	Render::BeginColor(Color(255, 128, 0, 255));
	Render::DrawRect(924, 0, 100, 100);
	Render::EndColor();
	Render::device.SetTexturing(true);
	
	_effCont.Draw();
	
	Render::BindFont("arial");
	Render::PrintString(924 + 100 / 2, 25, utils::lexical_cast(mouse_pos.x) + ", " + utils::lexical_cast(mouse_pos.y), 1.f, CenterAlign);
}

void MainSceneWidget::Update(float dt)
{
	UpdateCannon(dt);
	UpdateBubbles(dt);
	UpdateProjectiles(dt);
	
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
	
	float directionAngle = math::atan(mousePosition.y - _startPosition.y, mousePosition.x - _startPosition.x);
	float rotationAngle = (directionAngle * 180) / math::PI;

	ProjectilePtr projectilePtr = Projectile::Create();
	
	projectilePtr->SetPosition(CalculateProjectileStartPosition());
	projectilePtr->SetDirectionAngle(directionAngle);
	projectilePtr->SetRotationAngle(rotationAngle);

	_launchedProjectiles.push_back(projectilePtr);
}

FPoint MainSceneWidget::CalculateProjectileStartPosition() const
{
	float angle = (math::PI * _cannon->GetRotationAngle()) / 180;
	float cannonTextHeight = _cannon->GetTextureRect().Height();

	FPoint position(
		_startPosition.x + cannonTextHeight * math::cos(angle), 
		_startPosition.y + cannonTextHeight * math::sin(angle)
	);

	return position;
}

void MainSceneWidget::DestroyProjectile(const ProjectilePtr& projectile)
{
	const auto iter = std::find(_launchedProjectiles.begin(), _launchedProjectiles.end(), projectile);
	if (iter != _launchedProjectiles.end())
	{
		_launchedProjectiles.erase(iter);
	}
}

void MainSceneWidget::DrawBubbles()
{
	for (const auto& bubblePtr : _bubbles)
	{
		bubblePtr->Draw();
	}
}

void MainSceneWidget::UpdateBubbles(float dt)
{

}

void MainSceneWidget::DrawProjectiles()
{
	for (const auto& projectilePtr : _launchedProjectiles)
	{
		projectilePtr->Draw();
	}
}

void MainSceneWidget::UpdateProjectiles(float dt)
{
	FRect screenRect(0.f, Render::device.Width(), 0.f, Render::device.Height());

	std::vector<ProjectilePtr> projectilesToDestroy;

	for (const auto& projectilePtr : _launchedProjectiles)
	{
		projectilePtr->Update(dt);

		FPoint position = projectilePtr->GetPosition();
		float directionAngle = projectilePtr->GetDirectionAngle();

		position.x += math::cos(directionAngle) * (Speed * dt * 10);
		position.y += math::sin(directionAngle) * (Speed * dt * 10);

		projectilePtr->SetPosition(position);

		if (position.x < 0 || position.x >= screenRect.xEnd)
		{
			projectilePtr->OnCollideWithWall();
			continue;
		}

		if (!screenRect.Contains(position))
		{
			projectilesToDestroy.push_back(projectilePtr);
		}
	}

	for (auto& projectilePtr : projectilesToDestroy)
	{
		DestroyProjectile(projectilePtr);
		projectilePtr = nullptr;
	}
}

void MainSceneWidget::UpdateCannon(float dt)
{
	_cannon->Update(dt);

	FPoint mousePosition = static_cast<FPoint>(Core::mainInput.GetMousePos());

	if (mousePosition.x == 0.f && mousePosition.y == 0)
	{
		return;
	}

	float angle = (math::atan(mousePosition.y - _startPosition.y, mousePosition.x -_startPosition.x) * 180.0f) / math::PI;

	_cannon->SetRotationAngle(angle);
}