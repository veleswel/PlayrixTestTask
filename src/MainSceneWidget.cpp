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

const float MainSceneWidget::ProjectileSpeed = 250.f;
const float MainSceneWidget::MinBubbleSpeed = 50.f;
const float MainSceneWidget::MaxBubbleSpeed = 100.f;
const float MainSceneWidget::BubbleLaunchScreenOffset = 100.f;
const int MainSceneWidget::BubblesCount = 2;
const int MainSceneWidget::PlayTime = 2;

MainSceneWidget::MainSceneWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _cannon(nullptr)
	, _screenRect(0.f, Render::device.Width(), 0.f, Render::device.Height())
	, _startPosition(Render::device.Width() / 2.f, 0.f)
	, _projectilesTotalLaunch(0)
	, _shading(0.f)
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

	_background = Core::resourceManager.Get<Render::Texture>("background");
	
	_cannon.reset(new Cannon(_startPosition, 90.f));
	_cannon->SetAnchorPoint(FPoint(0.f, .5f));
}

void MainSceneWidget::Draw()
{
	GUI::Widget::Draw();

	GUI::Widget::Draw();
	Render::ShaderProgram* myshader = Core::resourceManager.Get<Render::ShaderProgram>("myshader");
	
	myshader->Bind();
	myshader->SetUniform("u_modelview", math::Matrix4::Identity);
	myshader->SetUniform("sampler", 0);
	myshader->SetUniform("multiplier", _shading);
	
	_background->Draw();
	
	DrawBubbles();
	DrawProjectiles();

	_cannon->Draw();
	
	for (const WallPtr& wall: _walls)
	{
		wall->Draw();
	}
	
	Render::BindFont("arial");
	Render::PrintString(10.f, _screenRect.Height() - 15.f, "Projectiles: " + utils::lexical_cast(_projectilesTotalLaunch), 1.5f, LeftAlign);
	Render::PrintString(10.f, _screenRect.Height() - 30.f, "Bubbles left: " + utils::lexical_cast(_bubbles.size()), 1.5f, LeftAlign);
	Render::PrintString(10.f, _screenRect.Height() - 45.f, "Timer: " + utils::lexical_cast(_timer_b.elapsed()), 1.5f, LeftAlign);

	_effCont.Draw();
	
	myshader->Unbind();
}

void MainSceneWidget::Update(float dt)
{
	GUI::Widget::Update(dt);

	_shading += dt;
	if (_shading >= 1.f)
	{
		_shading = 1.f;
	}
	
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
		if (!projPtr->IsNeedToCheckCollision())
		{
			projPtr->Update(dt);
			continue;
		}

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

	for (auto& projPtr: projectilesToDestroy)
	{
		RemoveProjectile(projPtr);
		projPtr.reset();
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

	if (publisher == "MenuWidget" && data == "startNewGame")
	{
		StartNewGame();
	}
	else if (publisher == "MenuWidget" && data == "continueGame")
	{
		Log::Debug("MenuWidget continueGame");
	}
	else if (publisher == "Layer" && data == "LayerInit")
	{
		_shading = 0.f;
		Log::Debug("Layer Init");
	}
	else if (publisher == "Layer" && data == "LayerDeinit")
	{
		Log::Debug("Layer Deinit");
	}
}

void MainSceneWidget::KeyPressed(int keyCode)
{
	if (keyCode == VK_ESCAPE)
	{
		_shading = 0.f;
		
		Core::mainScreen.popLayer();
		Core::mainScreen.pushLayer("MenuLayer");
	}
}

void MainSceneWidget::StartNewGame()
{
	_launchedProjectiles.clear();
	_bubbles.clear();

	LaunchBubbles();
	
	_timer_b.restart();
}

void MainSceneWidget::PlayWinnerEffects()
{
	
}

void MainSceneWidget::UpdateCannon(float dt)
{
	_cannon->Update(dt);
}

void MainSceneWidget::DrawProjectiles()
{
	for (const ProjectilePtr& projectile : _launchedProjectiles)
	{
		projectile->Draw();
	}
}

void MainSceneWidget::LaunchProjectile(const IPoint& position)
{
	const float angle = _cannon->GetRotationAngle();
	const float angleRad = Utils::DegreeToRadian(angle);
	const FPoint startPosition(CalculateProjectileStartPosition());
	const FPoint direction(math::cos(angleRad), math::sin(angleRad));

	if (direction.y < 0.f)
	{
		return;
	}

	ProjectilePtr projectile(new Projectile(startPosition, angle, direction.Normalized(), ProjectileSpeed));
	
	if (projectile->GetOBB().Overlaps(_walls[0]->GetOBB()))
	{
		projectile.reset();
		projectile = nullptr;
		return;
	}
	
	_launchedProjectiles.push_back(projectile);
	_projectilesTotalLaunch++;
}

const FPoint MainSceneWidget::CalculateProjectileStartPosition() const
{
	const float angle = Utils::DegreeToRadian(_cannon->GetRotationAngle());
	const float cannonTextHeight = _cannon->GetScaledTextureRect().Width() + 20.f;

	return FPoint(
		_startPosition.x + cannonTextHeight * math::cos(angle), 
		_startPosition.y + cannonTextHeight * math::sin(angle)
	);
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
	for (const BubblePtr& bubble : _bubbles)
	{
		bubble->Draw();
	}
}

void MainSceneWidget::LaunchBubbles()
{
	const float width = _screenRect.Width() - BubbleLaunchScreenOffset;
	const float height = _screenRect.Height() - BubbleLaunchScreenOffset;
	
	for (int i = 0; i < BubblesCount; ++i)
	{
		const FPoint position(math::random(BubbleLaunchScreenOffset, width), math::random(BubbleLaunchScreenOffset, height));
		const float speed = math::random(MinBubbleSpeed, MaxBubbleSpeed);
		const float angle = math::random(1.f + 0.f, 2 * math::PI - 1.f);
		
		const FPoint direction(math::cos(angle), math::sin(angle));

		BubblePtr bubblePtr(new Bubble(position, 0.f, direction.Normalized(), speed));
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
