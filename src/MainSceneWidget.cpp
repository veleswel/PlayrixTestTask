#include "stdafx.h"
#include "MainSceneWidget.hpp"
#include <boost/polymorphic_pointer_cast.hpp>

const float MainSceneWidget::ProjectileSpeed = 30.f;

const float MainSceneWidget::MinBubbleSpeed = 100.f;
const float MainSceneWidget::MaxBubbleSpeed = 200.f;
const float MainSceneWidget::BubbleLaunchScreenPrecision = 50.f;
const int MainSceneWidget::BubblesCount = 10;

MainSceneWidget::MainSceneWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _cannon(nullptr)
	, _screenRect(0.f, Render::device.Width(), 0.f, Render::device.Height())
	, _startPosition(FPoint(Render::device.Width() / 2, 0))
{
	Init();
}

void MainSceneWidget::Init()
{
	_walls = {
		std::make_shared<Wall>(_screenRect.xStart, _screenRect.yStart, _screenRect.xEnd, _screenRect.yStart), // bottom
		std::make_shared<Wall>(_screenRect.xEnd, _screenRect.yStart, _screenRect.xEnd, _screenRect.yEnd), // right
		std::make_shared<Wall>(_screenRect.xEnd, _screenRect.yEnd, _screenRect.xStart, _screenRect.yEnd), // top
		std::make_shared<Wall>(_screenRect.xStart, _screenRect.yEnd, _screenRect.xStart, _screenRect.yStart) // left
	};
	
	_cannon = Cannon::create();
	_cannon->SetPosition(_startPosition);
	_cannon->SetAnchorPoint(FPoint(0.f, .5f));
	_cannon->SetRotationAngle(90.f);
	
	LaunchBubbles();
}

void MainSceneWidget::Draw()
{
	const IPoint mouse_pos = Core::mainInput.GetMousePos();
	
	Render::device.SetTexturing(false);
	Render::BeginColor(Color(106, 126, 160, 255));
	Render::DrawRect(_screenRect.xStart, _screenRect.yStart, _screenRect.Width(), _screenRect.Height());
	Render::EndColor();
	Render::device.SetTexturing(true);
	
	DrawBubbles();
	DrawProjectiles();

	_cannon->Draw();
	
	for (auto wall: _walls)
	{
		wall->Draw();
	}
	
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

	QuadTree quad(0, _screenRect);
	quad.Clear();

	UpdateObjectsAndFillQuadTree(dt, quad);

	std::list<CollideableDelegatePtr> returnObjects;
	std::vector<ProjectilePtr> projectilesToDestroy;
	
	for (const ProjectilePtr& projPtr : _launchedProjectiles)
	{
		const math::Vector3& v = projPtr->GetVelocity();

		returnObjects.clear();
		quad.Retrieve(returnObjects, projPtr->GetAABB(), EColliderType::EBubble | EColliderType::EWall);

		bool isProjectileToDestroy = false;

		for (const CollideableDelegatePtr& object: returnObjects)
		{
			if (!projPtr->GetOBB().Overlaps(object->GetOBB()))
			{
				continue;
			}

			const EColliderType type = object->GetColliderType();
			if (type == EColliderType::EWall)
			{
				const WallPtr wall = boost::polymorphic_pointer_downcast<Wall>(object);
				if (!wall)
				{
					continue;
				}

				if (wall == _walls.at(0) || wall == _walls.at(2))
				{
					isProjectileToDestroy = true;
					break;
				}

				// Collision
				const math::Vector3& n = wall->GetNormal();
				const math::Vector3 u = v.DotProduct(n) * n;
				const math::Vector3 w = v - u;
				const math::Vector3 v1 = w - u;

				projPtr->SetVelocity(v1);
				projPtr->UpdatePosition(dt);
			}
			else if (type == EColliderType::EBubble)
			{
				const BubblePtr bubble = boost::polymorphic_pointer_downcast<Bubble>(object);
				if (!bubble)
				{
					continue;
				}

				DestroyBubble(bubble);
				isProjectileToDestroy = true;
				break;
			}
		}

		if (isProjectileToDestroy)
		{
			projectilesToDestroy.push_back(projPtr);
		}
	}

	for (const BubblePtr& bubblePtr : _bubbles)
	{
		returnObjects.clear();
		quad.Retrieve(returnObjects, bubblePtr->GetAABB(), EColliderType::EWall);

		const math::Vector3& v = bubblePtr->GetVelocity();

		for (const CollideableDelegatePtr& object : returnObjects)
		{
			if (!bubblePtr->GetOBB().Overlaps(object->GetOBB()))
			{
				continue;
			}

			const EColliderType type = object->GetColliderType();
			if (type == EColliderType::EWall)
			{
				const WallPtr wall = boost::polymorphic_pointer_downcast<Wall>(object);
				if (!wall)
				{
					continue;
				}

				const math::Vector3& n = wall->GetNormal();

				const math::Vector3 u = v.DotProduct(n) * n;
				const math::Vector3 w = v - u;
				const math::Vector3 v1 = w - u;

				bubblePtr->SetVelocity(v1);
				bubblePtr->UpdatePosition(dt);
			}
		}
	}

	for (auto& projPtr : projectilesToDestroy)
	{
		DestroyProjectile(projPtr);
		projPtr.reset();
		projPtr = nullptr;
	}

	_effCont.Update(dt);
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

void MainSceneWidget::LaunchProjectile(const IPoint& position)
{
	const FPoint mousePosition(position);
	
	const float angle = (math::atan(mousePosition.y - _startPosition.y, mousePosition.x - _startPosition.x) * 180) / math::PI;

	ProjectilePtr projectilePtr = Projectile::Create(ProjectileSpeed * 10.f);
	
	const FPoint startPosition(CalculateProjectileStartPosition());
	
	math::Vector3 start(startPosition.x, startPosition.y, 0);
	math::Vector3 end(mousePosition.x, mousePosition.y, 0);
	math::Vector3 velocity = end - start;
	velocity.Normalize();

	projectilePtr->SetPosition(startPosition);
	projectilePtr->SetRotationAngle(angle);
	projectilePtr->SetVelocity(velocity);

	const auto& obb = projectilePtr->GetOBB();
	
	if (obb.Overlaps(_walls[0]->GetOBB()))
	{
		projectilePtr = nullptr;
		return;
	}
	
	_launchedProjectiles.push_back(projectilePtr);
}

FPoint MainSceneWidget::CalculateProjectileStartPosition() const
{
	const float angle = (math::PI * _cannon->GetRotationAngle()) / 180.f;
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

void MainSceneWidget::LaunchBubbles()
{
	const float width = _screenRect.Width() - BubbleLaunchScreenPrecision;
	const float height = _screenRect.Height() - BubbleLaunchScreenPrecision;
	
	for (int i = 0; i < BubblesCount; ++i)
	{
		BubblePtr bubblePtr = Bubble::Create(math::random(MinBubbleSpeed, MaxBubbleSpeed));
		
		const FPoint position1(math::random(BubbleLaunchScreenPrecision, width), math::random(BubbleLaunchScreenPrecision, height));
		const FPoint position2(math::random(BubbleLaunchScreenPrecision, width), math::random(BubbleLaunchScreenPrecision, height));
		
		const float angle = (math::atan(position2.y - position1.y, position2.x - position1.x) * 180) / math::PI;
		
		const math::Vector3 start(position1.x, position1.y, 0);
		const math::Vector3 end(position2.x, position2.y, 0);
		math::Vector3 v = end - start;
		v.Normalize();
		
		bubblePtr->SetPosition(position1);
		bubblePtr->SetRotationAngle(angle);
		bubblePtr->SetVelocity(v);
		
		_bubbles.push_back(bubblePtr);
	}
}

void MainSceneWidget::DestroyBubble(const BubblePtr& bubble)
{
	const auto iter = std::find(_bubbles.begin(), _bubbles.end(), bubble);
	if (iter != _bubbles.end())
	{
		_bubbles.erase(iter);
	}
}

void MainSceneWidget::UpdateObjectsAndFillQuadTree(float dt, QuadTree& quad)
{
	for (const auto& projPtr: _launchedProjectiles)
	{
		projPtr->Update(dt);
		quad.Insert(projPtr);
	}

	for (const auto& bubblePtr: _bubbles)
	{
		bubblePtr->Update(dt);
		quad.Insert(bubblePtr);
	}

	for (const auto& wall: _walls)
	{
		quad.Insert(wall);
	}
}