#include "stdafx.h"
#include "MainSceneWidget.hpp"
#include "QuadTree.hpp"
#include "Wall.hpp"
#include "Projectile.hpp"
#include "Bubble.hpp"
#include "Cannon.hpp"
#include "Utils.hpp"
#include "CollisionUtils.hpp"
#include <boost/polymorphic_pointer_cast.hpp>

const float MainSceneWidget::ProjectileSpeed = 300.f;
const float MainSceneWidget::MinBubbleSpeed = 100.f;
const float MainSceneWidget::MaxBubbleSpeed = 150.f;
const float MainSceneWidget::BubbleLaunchScreenPrecision = 100.f;
const int MainSceneWidget::BubblesCount = 30;

MainSceneWidget::MainSceneWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _cannon(nullptr)
	, _screenRect(0.f, Render::device.Width(), 0.f, Render::device.Height())
	, _startPosition(FPoint(Render::device.Width() / 2.f, 0.f))
{
	Init();
}

void MainSceneWidget::Init()
{
	_walls = {
		WallPtr(new Wall(_screenRect.xStart, _screenRect.yStart, _screenRect.xEnd, _screenRect.yStart)), // bottom
		WallPtr(new Wall(_screenRect.xEnd, _screenRect.yStart, _screenRect.xEnd, _screenRect.yEnd)), // right
		WallPtr(new Wall(_screenRect.xEnd, _screenRect.yEnd, _screenRect.xStart, _screenRect.yEnd)), // top
		WallPtr(new Wall(_screenRect.xStart, _screenRect.yEnd, _screenRect.xStart, _screenRect.yStart)) // left
	};

	_cannon.reset(new Cannon(_startPosition, 90.f));
	_cannon->SetAnchorPoint(FPoint(0.f, .5f));
	
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
	
	for (const WallPtr& wall: _walls)
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

	FillQuadTree(quad);

	CheckAndResolveProjectilesCollisions(dt, quad);
	CheckAndResolveBubblesCollisions(dt, quad);

	_effCont.Update(dt);
}

void MainSceneWidget::CheckAndResolveProjectilesCollisions(float dt, QuadTree& quadTree)
{
	std::vector<ProjectilePtr> projectilesToDestroy;
	std::list<MovableObjectPtr> returnObjects;

	for (const ProjectilePtr& projPtr : _launchedProjectiles)
	{
		bool isProjectileToDestroy = false;
		const OBB2D& obb = projPtr->GetOBB();

		for (const WallPtr& wallPtr : _walls)
		{
			if (!wallPtr->GetOBB().Overlaps(obb))
			{
				continue;
			}

			if (wallPtr == _walls.at(0) || wallPtr == _walls.at(2))
			{
				isProjectileToDestroy = true;
				break;
			}

			CollisionUtils::ResolveFixedCollision(projPtr, wallPtr->GetNormal(), dt);
		}

		if (isProjectileToDestroy)
		{
			quadTree.Remove(projPtr);
			projectilesToDestroy.push_back(projPtr);
			continue;
		}

		returnObjects.clear();
		quadTree.Retrieve(returnObjects, projPtr, CollisionUtils::EColliderType::EBubble);

		for (const MovableObjectPtr& object : returnObjects)
		{
			if (!obb.Overlaps(object->GetOBB()))
			{
				continue;
			}

			const BubblePtr bubblePtr = boost::polymorphic_pointer_downcast<Bubble>(object);
			if (!bubblePtr)
			{
				continue;
			}

			quadTree.Remove(bubblePtr);
			RemoveBubble(bubblePtr);
			isProjectileToDestroy = true;
			break;
		}

		if (isProjectileToDestroy)
		{
			quadTree.Remove(projPtr);
			projectilesToDestroy.push_back(projPtr);
			continue;
		}

		projPtr->Update(dt);
	}

	for (auto& projPtr : projectilesToDestroy)
	{
		RemoveProjectile(projPtr);
		projPtr.reset();
		projPtr = nullptr;
	}
}

void MainSceneWidget::CheckAndResolveBubblesCollisions(float dt, QuadTree& quadTree)
{
	std::list<MovableObjectPtr> returnObjects;

	for (const BubblePtr& bubble : _bubbles)
	{
		returnObjects.clear();
		quadTree.Retrieve(returnObjects, bubble, CollisionUtils::EColliderType::EBubble);

		for (const MovableObjectPtr& object : returnObjects)
		{
			if (bubble == object || object->IsCollided())
			{
				continue;
			}

			const BubblePtr otherBubble = boost::polymorphic_pointer_downcast<Bubble>(object);
			if (!otherBubble)
			{
				continue;
			}

			float collisionTime = 0.f;
			if (CollisionUtils::DetectDynamicBubblesCollision(bubble, otherBubble, collisionTime, dt))
			{
				CollisionUtils::ResolveBubbleToBubbleCollision(bubble, otherBubble, collisionTime, dt);
			}
		}

		for (const WallPtr& wallPtr : _walls)
		{
			if (bubble->GetOBB().Overlaps(wallPtr->GetOBB()))
			{
				CollisionUtils::ResolveFixedCollision(bubble, wallPtr->GetNormal(), dt);
			}
		}

		bubble->Update(dt);
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
	const float angle = _cannon->GetRotationAngle();
	const float angleRad = Utils::DegreeToRadian(angle);
	const FPoint startPosition(CalculateProjectileStartPosition());
	const FPoint direction(ProjectileSpeed * math::cos(angleRad), ProjectileSpeed * math::sin(angleRad));

	if (direction.y < 0.f)
	{
		return;
	}

	ProjectilePtr projectilePtr(new Projectile(startPosition, angle, direction.Normalized(), ProjectileSpeed));
	
	if (projectilePtr->GetOBB().Overlaps(_walls[0]->GetOBB()))
	{
		projectilePtr.reset();
		projectilePtr = nullptr;
		return;
	}
	
	_launchedProjectiles.push_back(projectilePtr);
}

FPoint MainSceneWidget::CalculateProjectileStartPosition() const
{
	const float angle = (math::PI * _cannon->GetRotationAngle()) / 180.f;
	const float cannonTextHeight = _cannon->GetScaledTextureRect().Width();

	FPoint position(
		_startPosition.x + cannonTextHeight * math::cos(angle), 
		_startPosition.y + cannonTextHeight * math::sin(angle)
	);

	return position;
}

void MainSceneWidget::RemoveProjectile(const ProjectilePtr& projectile)
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
		const FPoint position(math::random(BubbleLaunchScreenPrecision, width), math::random(BubbleLaunchScreenPrecision, height));
		const float speed = math::random(MinBubbleSpeed, MaxBubbleSpeed);
		const float angle = math::random(1.f + 0.f, 2 * math::PI - 1.f);
		
		const FPoint direction(speed * math::cos(angle), speed * math::sin(angle));

		BubblePtr bubblePtr(new Bubble(position, Utils::RadianToDegree(angle), direction.Normalized(), speed));
		_bubbles.push_back(bubblePtr);
	}
}

void MainSceneWidget::RemoveBubble(const BubblePtr& bubble)
{
	const auto iter = std::find(_bubbles.begin(), _bubbles.end(), bubble);
	if (iter != _bubbles.end())
	{
		_bubbles.erase(iter);
	}
}

void MainSceneWidget::FillQuadTree(QuadTree& quad)
{
	for (const auto& projPtr: _launchedProjectiles)
	{
		projPtr->SetCollided(false);
		quad.Insert(projPtr);
	}

	for (const auto& bubblePtr: _bubbles)
	{
		bubblePtr->SetCollided(false);
		quad.Insert(bubblePtr);
	}
}
