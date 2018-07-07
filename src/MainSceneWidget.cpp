#include "stdafx.h"
#include "MainSceneWidget.hpp"
#include "Utils.hpp"
#include <boost/polymorphic_pointer_cast.hpp>

const float MainSceneWidget::ProjectileSpeed = 200.f;
const float MainSceneWidget::MinBubbleSpeed = 100.f;
const float MainSceneWidget::MaxBubbleSpeed = 150.f;
const float MainSceneWidget::BubbleLaunchScreenPrecision = 100.f;
const int MainSceneWidget::BubblesCount = 100;

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

	FillQuadTree(dt, quad);

	std::list<CollideableDelegatePtr> returnObjects;
	std::vector<ProjectilePtr> projectilesToDestroy;
	
	for (const ProjectilePtr& projPtr : _launchedProjectiles)
	{
		bool isProjectileToDestroy = false;
		const math::Vector3& v = projPtr->GetVelocity();
		const OBB2D& obb = projPtr->GetOBB();
		const FRect& aabb = projPtr->GetAABB();

		for (const WallPtr& wallPtr : _walls)
		{
			float time = 0.f;
			
			/*if (!wallPtr->GetOBB().Overlaps(obb))
			{
				continue;
			}*/

			if (!Utils::SweptAABB(aabb, wallPtr->GetAABB(), v, time))
			{
				continue;
			}

			if (wallPtr == _walls.at(0) || wallPtr == _walls.at(2))
			{
				isProjectileToDestroy = true;
				break;
			}

			Utils::ResolveFixedCollision(projPtr, wallPtr->GetNormal());
			projPtr->UpdatePosition(dt * (1.f - time));
		}

		if (isProjectileToDestroy)
		{
			quad.Remove(projPtr);
			projectilesToDestroy.push_back(projPtr);
			continue;
		}

		returnObjects.clear();
		quad.Retrieve(returnObjects, projPtr, EColliderType::EBubble);

		for (const CollideableDelegatePtr& object: returnObjects)
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

			quad.Remove(bubblePtr);
			RemoveBubble(bubblePtr);
			isProjectileToDestroy = true;
			break;
		}

		if (isProjectileToDestroy)
		{
			quad.Remove(projPtr);
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

	for (const BubblePtr& bubblePtr : _bubbles)
	{
		const math::Vector3& velocity = bubblePtr->GetVelocity();
		
		const OBB2D& obb = bubblePtr->GetOBB();
		const FRect aabb = bubblePtr->GetAABB();
				
		returnObjects.clear();
		quad.Retrieve(returnObjects, bubblePtr, EColliderType::EBubble);
		
		for (const CollideableDelegatePtr& objectPtr : returnObjects)
		{
			const OBB2D& otherOBB = objectPtr->GetOBB();

			if (bubblePtr == objectPtr || !obb.Overlaps(otherOBB) || objectPtr->isCollided())
			{
				continue;
			}

			const BubblePtr otherBubblePtr = boost::polymorphic_pointer_downcast<Bubble>(objectPtr);
			if (!otherBubblePtr)
			{
				continue;
			}

			float r1 = bubblePtr->GetRadius();
			float r2 = otherBubblePtr->GetRadius();

			float vx1 = bubblePtr->GetVelocity().x;
			float vy1 = bubblePtr->GetVelocity().y;

			float vx2 = otherBubblePtr->GetVelocity().x;
			float vy2 = otherBubblePtr->GetVelocity().y;

			float time = 0.f;

			if (Utils::Collision2D(r1, r2, bubblePtr->GetPosition(), otherBubblePtr->GetPosition(), vx1, vy1, vx2, vy2, time))
			{
				//bubblePtr->SetVelocity(math::Vector3(vx1, vy1, 0.f).Normalized());
				//bubblePtr->UpdatePosition(dt * (1 - time));
				//bubblePtr->UpdatePosition(dt);

				//otherBubblePtr->SetVelocity(math::Vector3(vx2, vy2, 0.f).Normalized());
				//otherBubblePtr->UpdatePosition(dt * (1 - time));
				//otherBubblePtr->UpdatePosition(dt);

				bubblePtr->SetCollided(true);
				otherBubblePtr->SetCollided(true);
			}
		}
		
		for (const WallPtr& wallPtr : _walls)
		{
			float time = 0.f;

			/*if (!obb.Overlaps(wallPtr->GetOBB()))
			{
				continue;
			}*/

			if (Utils::SweptAABB(aabb, wallPtr->GetAABB(), velocity, time))
			{
				Utils::ResolveFixedCollision(bubblePtr, wallPtr->GetNormal());
				//bubblePtr->UpdatePosition(dt * (1.f - time));
				//bubblePtr->UpdatePosition(dt);
			}
		}

		bubblePtr->Update(dt);
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
	const float angle = _cannon->GetRotationAngle();
	const float angleRad = Utils::DegreeToRadian(angle);
	const FPoint startPosition(CalculateProjectileStartPosition());
	math::Vector3 velocity(ProjectileSpeed * math::cos(angleRad), ProjectileSpeed * math::sin(angleRad), 0.f);

	if (velocity.y < 0.f)
	{
		return;
	}

	ProjectilePtr projectilePtr = std::make_shared<Projectile>(startPosition, angle, velocity.Normalized(), ProjectileSpeed);
	
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
		
		const math::Vector3 velocity(speed * math::cos(angle), speed * math::sin(angle), 0.f);

		BubblePtr bubblePtr = std::make_shared<Bubble>(position, Utils::RadianToDegree(angle), velocity.Normalized(), speed);
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

void MainSceneWidget::FillQuadTree(float dt, QuadTree& quad)
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
