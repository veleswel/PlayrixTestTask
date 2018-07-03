#include "stdafx.h"
#include "MainSceneWidget.hpp"

const float MainSceneWidget::ProjectileSpeed = 25.f;
const float MainSceneWidget::BubbleSpeed = 25.f;
const int MainSceneWidget::BubblesCount = 0;

MainSceneWidget::MainSceneWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _timer(0.f)
	, _cannon(nullptr)
	, _startPosition(FPoint(Render::device.Width() / 2, 0))
{
	Init();
}

void MainSceneWidget::Init()
{
	_cannon = Cannon::create();
	_cannon->SetPosition(_startPosition);
	_cannon->SetAnchorPoint(FPoint(0.f, .5f));
	_cannon->SetRotationAngle(90.f);
	
	LaunchBubbles();
}

void MainSceneWidget::Draw()
{
	const IPoint mouse_pos = Core::mainInput.GetMousePos();
	
	const FRect screenRect = GetScreenRect();
	
	Render::device.SetTexturing(false);
	Render::BeginColor(Color(106, 126, 160, 255));
	Render::DrawRect(0, 0, screenRect.Width(), screenRect.Height());
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
	
	while (_timer > 1.f)
	{
		_timer -= 1.f;
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

}

void MainSceneWidget::UpdateCannon(float dt)
{
	_cannon->Update(dt);
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
	const FRect screenRect = GetScreenRect();
	
	std::vector<ProjectilePtr> projectilesToDestroy;
	
	for (const auto& projectilePtr : _launchedProjectiles)
	{
		projectilePtr->Update(dt);
		const FRect bbox = projectilePtr->GetBoundingBox();

		if (bbox.xStart <= screenRect.xStart || bbox.xEnd >= screenRect.xEnd)
		{
			projectilePtr->InvertVelocityX();
			projectilePtr->UpdatePosition(dt);
		}
		
		if (bbox.yStart <= screenRect.yStart || bbox.yEnd >= screenRect.yEnd)
		{
			projectilesToDestroy.push_back(projectilePtr);
		}
	}
	
	for (auto& projectilePtr: projectilesToDestroy)
	{
		DestroyProjectile(projectilePtr);
		projectilePtr = nullptr;
	}
}

void MainSceneWidget::LaunchProjectile(const IPoint& position)
{
	const FPoint mousePosition(position);
	
	const float directionAngle = math::atan(mousePosition.y - _startPosition.y, mousePosition.x - _startPosition.x);
	const float rotationAngle = (directionAngle * 180) / math::PI;

	ProjectilePtr projectilePtr = Projectile::Create(ProjectileSpeed * 10.f);
	
	math::Vector3 start(_startPosition.x, _startPosition.y, 0);
	math::Vector3 end(mousePosition.x, mousePosition.y, 0);
	math::Vector3 velocity = end - start;
	velocity.Normalize();

	projectilePtr->SetPosition(CalculateProjectileStartPosition());
	projectilePtr->SetRotationAngle(rotationAngle);
	projectilePtr->SetVelocity(velocity);

	//projectilePtr->SetDirectionAngle(directionAngle);

	_launchedProjectiles.push_back(projectilePtr);
}

FPoint MainSceneWidget::CalculateProjectileStartPosition() const
{
	const float angle = (math::PI * _cannon->GetRotationAngle()) / 180;
	const float cannonTextHeight = _cannon->GetTextureRect().Height();

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
	const FRect screenRect = GetScreenRect();;
	
	for (const auto& bubblePtr: _bubbles)
	{
		bubblePtr->Update(dt);
		const FRect bbox = bubblePtr->GetBoundingBox();

		const float dirAngle = bubblePtr->GetDirectionAngle();
		float newDirAngle = dirAngle;

		if (bbox.xStart <= screenRect.xStart || bbox.xEnd >= screenRect.xEnd)
		{
			newDirAngle = math::PI - dirAngle;
		}
		
		if (bbox.yStart <= screenRect.yStart || bbox.yEnd >= screenRect.yEnd)
		{
			newDirAngle = 2 * math::PI - dirAngle;
		}

		if (dirAngle != newDirAngle)
		{
			bubblePtr->SetDirectionAngle(newDirAngle);
			bubblePtr->UpdatePosition(dt);
		}
	}
}

void MainSceneWidget::LaunchBubbles()
{
	const FRect screenRect = GetScreenRect();
	const float minDirAngle = 0.f;
	const float maxDirAngle = 2 * math::PI;
	
	for (int i = 0; i < BubblesCount; ++i)
	{
		BubblePtr bubblePtr = Bubble::Create(BubbleSpeed * 10.f);
		const FPoint position = math::random(screenRect.LeftBottom(), screenRect.RightTop());
		const float directionAngle = math::random(minDirAngle, maxDirAngle);
		
		bubblePtr->SetPosition(position);
		bubblePtr->SetDirectionAngle(directionAngle);
		
		_bubbles.push_back(bubblePtr);
	}
}

FRect MainSceneWidget::GetScreenRect() const
{
	return FRect(0.f, Render::device.Width(), 0.f, Render::device.Height());
}

EColissionSide MainSceneWidget::DoBoxesCollide(const FRect& bbox1, const FRect& bbox2)
{
	const float w = 0.5 * (bbox1.Width() + bbox2.Width());
	const float h = 0.5 * (bbox1.Height() + bbox2.Height());

	const float dx = bbox1.CenterPoint().x - bbox2.CenterPoint().x;
	const float dy = bbox1.CenterPoint().y - bbox2.CenterPoint().y;

	if (abs(dx) <= w && abs(dy) <= h)
	{
		/* collision! */
		float wy = w * dy;
		float hx = h * dx;

		if (wy > hx)
		{
			if (wy > -hx)
			{
				/* collision at the top */
				return EColissionSide::ETop;
			}
			else
			{
				/* on the left */
				return EColissionSide::ELeft;
			}
		}
		else
		{
			if (wy > -hx)
			{
				/* on the right */
				return EColissionSide::ERight;
			}
			else
			{
				/* at the bottom */
				return EColissionSide::EBottom;
			}
		}
	}

	return EColissionSide::ENone;
}