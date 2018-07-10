#include "stdafx.h"
#include "MainSceneWidget.hpp"
#include "QuadTree.hpp"
#include "Wall.hpp"
#include "Projectile.hpp"
#include "Bubble.hpp"
#include "Cannon.hpp"
#include "Utils.hpp"
#include "CollisionUtils.hpp"
#include "GameStateHandler.hpp"
#include <boost/polymorphic_pointer_cast.hpp>

const float MainSceneWidget::MinBubbleSpeed = 50.f;
const float MainSceneWidget::MaxBubbleSpeed = 100.f;
const float MainSceneWidget::BubbleLaunchScreenOffset = 100.f;

MainSceneWidget::MainSceneWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _cannon(nullptr)
	, _screenRect(0.f, Render::device.Width(), 0.f, Render::device.Height())
	, _startPosition(Render::device.Width() / 2.f, -30.f)
	, _projectilesTotalLaunch(0)
	, _timeLeft(0.f)
	, _playTime(0.f)
	, _effectContainer(nullptr)
{
	Core::Timer::Sync();
	Init();
}

void MainSceneWidget::Init()
{
	_effectContainer.reset(new EffectsContainer);
	
	_walls = {
		WallPtr(new Wall(_screenRect.xStart, _screenRect.yStart, _screenRect.xEnd, _screenRect.yStart)), // bottom
		WallPtr(new Wall(_screenRect.xEnd, _screenRect.yStart, _screenRect.xEnd, _screenRect.yEnd)), // right
		WallPtr(new Wall(_screenRect.xEnd, _screenRect.yEnd, _screenRect.xStart, _screenRect.yEnd)), // top
		WallPtr(new Wall(_screenRect.xStart, _screenRect.yEnd, _screenRect.xStart, _screenRect.yStart)) // left
	};

	_background = Core::resourceManager.Get<Render::Texture>("background");
	
	_cannon.reset(new Cannon(_startPosition, 90.f));
	_cannon->SetAnchorPoint(FPoint(0.f, .5f));
	
	ReadInitialData();
}

void MainSceneWidget::ReadInitialData()
{
	boost::system::error_code err;
	auto stream = Core::fileSystem.OpenRead("start.txt", err);
	
	assert(err == 0);
	
	IO::TextReader reader(stream.get());
	
	_startBubblesCount = ReadValue<int>(&reader, "Targets");
	_projectileSpeed = ReadValue<float>(&reader, "Speed");
	_playTime = ReadValue<float>(&reader, "Time");
}

void MainSceneWidget::Draw()
{
	GUI::Widget::Draw();

	_background->Draw();
	
	DrawBubbles();
	DrawProjectiles();

	_cannon->Draw();
	
	for (const WallPtr& wall: _walls)
	{
		wall->Draw();
	}
	
	_effectContainer->Draw();
	
	Render::BindFont("arial");
	Render::PrintString(10.f, _screenRect.Height() - 10.f, "Projectiles launched: " + utils::lexical_cast(_projectilesTotalLaunch), 1.5f, LeftAlign);
	Render::PrintString(10.f, _screenRect.Height() - 30.f, "Bubbles left: " + utils::lexical_cast(_bubbles.size()), 1.5f, LeftAlign);
	Render::PrintString(10.f, _screenRect.Height() - 50.f, "Time left: " + utils::lexical_cast(_timeLeft), 1.5f, LeftAlign);
}

void MainSceneWidget::Update(float dt)
{
	GUI::Widget::Update(dt);
	
	UpdateCannon(dt);
	UpdateGameItems(dt);

	_effectContainer->Update(dt);

	const EGameState currentState = GameStateHandler::GetInstance().GetGameState();
	
	if (currentState != EGameState::EFinish && currentState != EGameState::EPause)
	{
		_timeLeft = math::ceil(_playTime - _timer.getElapsedTime()) + 0.f;
		if (_timeLeft <= 0.f)
		{
			_timeLeft = 0.f;
			Loose();
		}
	}
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
	
	if (_bubbles.empty() && GameStateHandler::GetInstance().GetGameState() != EGameState::EFinish)
	{
		Win();
	}

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
	LaunchProjectile();	
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

	const EGameState currentState = GameStateHandler::GetInstance().GetGameState();
	
	if (publisher == "MenuWidget" && data == "startNewGame")
	{
		StartNewGame();
	}
	else if (publisher == "MenuWidget" && data == "continueGame")
	{
		ResumeGame();
	}
	else if (publisher == "Layer" && data == "LayerInit")
	{
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
		PauseGame();
	}
}

void MainSceneWidget::StartNewGame()
{
	_launchedProjectiles.clear();
	_bubbles.clear();
	_effectContainer->KillAllEffects();
	_projectilesTotalLaunch = 0;

	LaunchBubbles();
	
	_timeLeft = _playTime;
	_timer.Start();

	GameStateHandler::GetInstance().SetGameState(EGameState::EPlaying);
}

void MainSceneWidget::PauseGame()
{
	_timer.Pause();

	auto& instance = GameStateHandler::GetInstance();
	if (instance.GetGameState() != EGameState::EFinish)
	{
		instance.SetGameState(EGameState::EPause);
	}
	
	Core::mainScreen.popLayer();
	Core::mainScreen.pushLayer("MenuLayer");
}

void MainSceneWidget::ResumeGame()
{
	const EGameState currentState = GameStateHandler::GetInstance().GetGameState();
	
	if (currentState != EGameState::EFinish)
	{
		_timer.Resume();
		GameStateHandler::GetInstance().SetGameState(EGameState::EPlaying);
	}
}

void MainSceneWidget::Win()
{
	_timer.Pause();
	GameStateHandler::GetInstance().SetGameState(EGameState::EFinish);
}

void MainSceneWidget::Loose()
{
	_timer.Pause();
	GameStateHandler::GetInstance().SetGameState(EGameState::EFinish);
}

void MainSceneWidget::UpdateCannon(float dt)
{
	_cannon->Update(dt);
}

void MainSceneWidget::UpdateGameItems(float dt)
{
	QuadTree quad(0, _screenRect);
	quad.Clear();

	FillQuadTree(quad);

	CheckAndResolveProjectilesCollisions(dt, quad);
	CheckAndResolveBubblesCollisions(dt, quad);
}

void MainSceneWidget::DrawProjectiles()
{
	for (const ProjectilePtr& projectile : _launchedProjectiles)
	{
		projectile->Draw();
	}
}

void MainSceneWidget::LaunchProjectile()
{
	const EGameState currentState = GameStateHandler::GetInstance().GetGameState();
	
	if (currentState == EGameState::EFinish || currentState == EGameState::EPause)
	{
		return;
	}

	const float angle = _cannon->GetRotationAngle();
	const float angleRad = Utils::DegreeToRadian(angle);
	const FPoint startPosition(CalculateProjectileStartPosition());
	const FPoint direction(math::cos(angleRad), math::sin(angleRad));

	if (direction.y < 0.f)
	{
		return;
	}

	ProjectilePtr projectile(new Projectile(startPosition, angle, direction.Normalized(), _projectileSpeed, _effectContainer));
	
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

void some(float max, float min)
{
	std::vector<FPoint> points;
	float yMax = max;
	float yMin = min;
	float r = (max - min) / 2;
	yMin += fmod(yMax - yMin, 1) / 2;
	for (float y = yMin; y < yMax; y++)
	{
		float xMax = math::cos(asinf(y/r)) * r;
		float xMin = -xMax;
		xMin += fmod(xMax-xMin, 1)/2;
		for (float x = xMin; x < xMax; x++)
		{
			points.push_back(FPoint(x + math::random(-r/4, r/4), y + math::random(-r/4, r/4)));
		}
	}
}

void MainSceneWidget::LaunchBubbles()
{
	const float width = _screenRect.Width() - BubbleLaunchScreenOffset;
	const float height = _screenRect.Height() - BubbleLaunchScreenOffset;
	
	for (int i = 0; i < _startBubblesCount; ++i)
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
