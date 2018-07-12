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
const float MainSceneWidget::CannonPositionYOffset = -30.f;
const float MainSceneWidget::InfoTextXOffset = 25.f;
const float MainSceneWidget::LaunchedTextYOffset = 15.f;
const float MainSceneWidget::BubblesTextYOffset = 45.f;
const float MainSceneWidget::TimeTextYOffset = 75.f;
const float MainSceneWidget::FinishImageYOffset = 25.f;
const float MainSceneWidget::MenuEnterTextYOffset = 50.f;

MainSceneWidget::MainSceneWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _cannon(nullptr)
	, _screenRect(0.f, Render::device.Width(), 0.f, Render::device.Height())
	, _startPosition(Render::device.Width() / 2.f, CannonPositionYOffset)
	, _projectilesTotalLaunched(0)
	, _timeLeft(0.f)
	, _playTime(0.f)
	, _effectContainer(nullptr)
	, _gameFinishImage(nullptr)
	, _gameStateHandlerRef(GameStateHandler::GetInstance())
	, _gameFinishPosition(0, 0)
{
	Core::Timer::Sync();
	Init();
}

void MainSceneWidget::Init()
{
	_effectContainer.reset(new EffectsContainer);
	
	
	// Создаем стены при старте
	_walls = {
		WallPtr(new Wall(_screenRect.xStart, _screenRect.yStart, _screenRect.xEnd, _screenRect.yStart)), // низ
		WallPtr(new Wall(_screenRect.xEnd, _screenRect.yStart, _screenRect.xEnd, _screenRect.yEnd)), // право
		WallPtr(new Wall(_screenRect.xEnd, _screenRect.yEnd, _screenRect.xStart, _screenRect.yEnd)), // верх
		WallPtr(new Wall(_screenRect.xStart, _screenRect.yEnd, _screenRect.xStart, _screenRect.yStart)) // лево
	};

	_background = Core::resourceManager.Get<Render::Texture>("background");
	
	// Создаем пушку
	_cannon.reset(new Cannon(_startPosition, 90.f));
	_cannon->SetAnchorPoint(FPoint(0.f, .5f));
	
	ReadInitialData();
}

void MainSceneWidget::ReadInitialData()
{
	boost::system::error_code err;
	const auto stream = Core::fileSystem.OpenRead("start.txt", err);
	
	if (err != 0)
	{
		Log::Error("Failed to open file with initial data");
		assert(false);
	}
	
	IO::TextReader reader(stream.get());
	
	// Важен порядок вычитывания данных из файла
	_startBubblesCount = ReadLineAndGetValue<int>(&reader, "Targets");
	_projectileSpeed = ReadLineAndGetValue<float>(&reader, "Speed");
	_playTime = ReadLineAndGetValue<float>(&reader, "Time");
}

void MainSceneWidget::Draw()
{
	GUI::Widget::Draw();

	_background->Draw();
	
	_effectContainer->Draw();
	
	DrawBubbles();
	DrawProjectiles();

	_cannon->Draw();

	Render::BindFont("tahomabold");

	// Если игра закончилась, рисуем картинку финиша
	if (_gameStateHandlerRef.GetGameState() == EGameState::EFinish && _gameFinishImage)
	{
		DrawFinishImageAndText();
	}
	
	// Рисуем статы
	Render::PrintString(InfoTextXOffset,
						_screenRect.Height() - LaunchedTextYOffset,
						"Projectiles launched: " + utils::lexical_cast(_projectilesTotalLaunched),
						1.f,
						LeftAlign);
	
	Render::PrintString(InfoTextXOffset,
						_screenRect.Height() - BubblesTextYOffset,
						"Bubbles left: " + utils::lexical_cast(_bubbles.size()),
						1.f,
						LeftAlign);
	
	Render::PrintString(InfoTextXOffset,
						_screenRect.Height() - TimeTextYOffset,
						"Time left: " + utils::lexical_cast(_timeLeft),
						1.f,
						LeftAlign);
}

void MainSceneWidget::Update(float dt)
{
	GUI::Widget::Update(dt);

	UpdateCannon(dt);
	UpdateGameItems(dt);

	_effectContainer->Update(dt);

	// Проверяем текущее состояние игры и оставшееся время
	// Если времени больше не осталось - проигрыш
	const EGameState currentState = _gameStateHandlerRef.GetGameState();
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

// Метод, который проверяет и разрешает столкновения снарядов с шарами и краями экрана
void MainSceneWidget::CheckAndResolveProjectilesCollisions(float dt, QuadTree& quadTree)
{
	std::vector<ProjectilePtr> projectilesToDestroy;
	std::list<MovableObjectPtr> returnObjects;

	for (const ProjectilePtr& projPtr : _launchedProjectiles)
	{
		bool isProjectileToDestroy = false;
		const OBB& obb = projPtr->GetOBB();

		// Стены перебираем обычным брутфорсом, так как их всего 4, и нет смысла помещать их в дерево
		for (const WallPtr& wallPtr : _walls)
		{
			if (!wallPtr->GetOBB().Overlaps(obb))
			{
				continue;
			}

			// Если снаряд столкнулся с верхней или нижней стеной, помечаем его к удалению
			if (wallPtr == _walls.at(0) || wallPtr == _walls.at(2))
			{
				isProjectileToDestroy = true;
				break;
			}

			// Иначе разрешаем столкновение со стеной как статическим объектом
			CollisionUtils::ResolveFixedCollision(projPtr, wallPtr->GetNormal(), dt);
		}

		// Если снаряд помечен к удалению, убираем его из дерева
		if (isProjectileToDestroy)
		{
			quadTree.Remove(projPtr);
			projectilesToDestroy.push_back(projPtr);
			continue;
		}

		// Проверяем столкновение снаряда с шарами
		returnObjects.clear();
		
		// Получаем все шары, с которыми снаряд преположительно может столнкуться
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

			// Если снаряд столкнулся с шаром, помечаем снаряд к удалению
			// Уничтожаем шар и проигрываем эффект уничтожения шара
			const FPoint bubblePosition = bubblePtr->GetPosition();
			
			ParticleEffectPtr burst = _effectContainer->AddEffect("BubbleBurst");
			burst->posX = bubblePosition.x + 0.f;
			burst->posY = bubblePosition.y + 0.f;
			burst->Reset();
			
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

	// Удаляем все помеченные снаряды
	for (auto& projPtr: projectilesToDestroy)
	{
		RemoveProjectile(projPtr);
		projPtr.reset();
	}
}

// Метод, который проверяет и разрешает столкновения шаров с краями экрана и друг с другом
void MainSceneWidget::CheckAndResolveBubblesCollisions(float dt, QuadTree& quadTree)
{
	if (_bubbles.empty() && _gameStateHandlerRef.GetGameState() != EGameState::EFinish)
	{
		Win();
	}

	std::list<MovableObjectPtr> returnObjects;

	for (const BubblePtr& bubble : _bubbles)
	{
		returnObjects.clear();
		
		// Получаем из дерева все шары, с которыми текущий шар предположительно может столкнуться
		quadTree.Retrieve(returnObjects, bubble, CollisionUtils::EColliderType::EBubble);

		for (const MovableObjectPtr& object : returnObjects)
		{
			// Если это тот же самый объект, или он уже сталкивался на текущей итерации игрового цикла,
			// переходим к следующему
			
			if (bubble == object || object->IsCollided())
			{
				continue;
			}

			const BubblePtr otherBubble = boost::polymorphic_pointer_downcast<Bubble>(object);
			if (!otherBubble)
			{
				continue;
			}

			// Проверяем столкновение и рассчитываем примерное время до столкновения
			float collisionTime = 0.f;
			if (CollisionUtils::DetectDynamicBubblesCollision(bubble, otherBubble, collisionTime, dt))
			{
				// Если шары столкнуться, то разрешаем столкновение
				CollisionUtils::ResolveBubbleToBubbleCollision(bubble, otherBubble, collisionTime, dt);
			}
		}

		// Проверяем столкновение со стенами
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
	// Запускаем снаряд по клику мыши
	LaunchProjectile();	
	return false;
}

void MainSceneWidget::AcceptMessage(const Message& message)
{
	const std::string& publisher = message.getPublisher();
	const std::string& data = message.getData();

	const EGameState currentState = _gameStateHandlerRef.GetGameState();
	
	if (publisher == "MenuWidget" && data == "startNewGame")
	{
		StartNewGame();
	}
	else if (publisher == "MenuWidget" && data == "continueGame")
	{
		ResumeGame();
	}
}

void MainSceneWidget::KeyPressed(int keyCode)
{
	if (keyCode == VK_ESCAPE)
	{
		// Ставим игру на паузу по нажатию на Escape
		PauseGame();
	}
}

void MainSceneWidget::StartNewGame()
{
	// Уничтожаем все движущиеся объекты,
	// сбрасываем все счетчики и таймеры и запускаем пузыри
	
	_launchedProjectiles.clear();
	_bubbles.clear();
	_effectContainer->KillAllEffects();
	_projectilesTotalLaunched = 0;

	_timeLeft = _playTime;
	_timer.Start();

	_gameStateHandlerRef.SetGameState(EGameState::EPlaying);
	_gameFinishImage = nullptr;
	
	LaunchBubbles();
}

void MainSceneWidget::PauseGame()
{
	_timer.Pause();

	if (_gameStateHandlerRef.GetGameState() != EGameState::EFinish)
	{
		_gameStateHandlerRef.SetGameState(EGameState::EPause);
	}
	
	// Показываем меню
	Core::mainScreen.popLayer();
	Core::mainScreen.pushLayer("MenuLayer");
}

void MainSceneWidget::ResumeGame()
{
	const EGameState currentState = _gameStateHandlerRef.GetGameState();
	if (currentState != EGameState::EFinish)
	{
		_timer.Resume();
		_gameStateHandlerRef.SetGameState(EGameState::EPlaying);
	}
}

void MainSceneWidget::Win()
{
	// Создаем картинку в зависимости от результата игры
	_gameFinishImage = Core::resourceManager.Get<Render::Texture>("won");
	FinishGame();
}

void MainSceneWidget::Loose()
{
	_gameFinishImage = Core::resourceManager.Get<Render::Texture>("lost");
	FinishGame();
}

void MainSceneWidget::FinishGame()
{
	_timer.Pause();
	_launchedProjectiles.clear();
	_gameStateHandlerRef.SetGameState(EGameState::EFinish);
	
	// Рассчитываем позицию картинки на экране
	CalculateFinishImagePosition();
}

void MainSceneWidget::DrawFinishImageAndText()
{
	// Рисуем картинку окончания игры и текст
	
	Render::device.PushMatrix();
	Render::device.MatrixTranslate(_gameFinishPosition);
	_gameFinishImage->Draw();
	Render::device.PopMatrix();
	
	Render::PrintString(_screenRect.Width() / 2,
						_screenRect.Height() / 2 - MenuEnterTextYOffset,
						"Press escape to enter menu",
						1.f,
						CenterAlign);
}

void MainSceneWidget::CalculateFinishImagePosition()
{
	const IRect textureRect(_gameFinishImage->getBitmapRect());
	_gameFinishPosition = IPoint(_screenRect.Width() / 2 - textureRect.Width() / 2,
								 _screenRect.Height() / 2 - textureRect.Height() / 2 + FinishImageYOffset);
}

void MainSceneWidget::UpdateCannon(float dt)
{
	_cannon->Update(dt);
}

void MainSceneWidget::UpdateGameItems(float dt)
{
	// Создаем объект quadtree
	QuadTree quad(0, _screenRect);
	quad.Clear();

	// Заполняем его
	FillQuadTree(quad);

	// Проверяем столкновения, разрешаем их и апдейтим объекты
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
	const EGameState currentState = _gameStateHandlerRef.GetGameState();
	
	if (currentState == EGameState::EFinish || currentState == EGameState::EPause)
	{
		return;
	}

	// Рассчитываем параметры запуска снаряда на основе угла повороты пушки и её позиции
	const float angle = _cannon->GetRotationAngle();
	const float angleRad = Utils::DegreeToRadian(angle);
	const FPoint startPosition(CalculateProjectileStartPosition());
	const FPoint direction(math::cos(angleRad), math::sin(angleRad));

	// Не можем запустить снаряд вниз
	if (direction.y < 0.f)
	{
		return;
	}

	const ProjectilePtr projectile(new Projectile(startPosition, angle, direction.Normalized(), _projectileSpeed, _effectContainer));
	_launchedProjectiles.push_back(projectile);
	_projectilesTotalLaunched++;
}

// Метод рассчитывает стартовую позицию снаряда на основе угла поворота пушки и высоты её текстуры
const FPoint MainSceneWidget::CalculateProjectileStartPosition() const
{
	const float angle = Utils::DegreeToRadian(_cannon->GetRotationAngle());
	const float cannonTextHeight = _cannon->GetScaledTextureRect().Width();

	return FPoint(_startPosition.x + cannonTextHeight * math::cos(angle), _startPosition.y + cannonTextHeight * math::sin(angle));
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
	// Ограничиваем область запуска пузырей на экране
	const float width = _screenRect.Width() - BubbleLaunchScreenOffset;
	const float height = _screenRect.Height() - BubbleLaunchScreenOffset;
	
	for (int i = 0; i < _startBubblesCount; ++i)
	{
		// Рандомно создаем позицию
		const FPoint position(math::random(BubbleLaunchScreenOffset, width), math::random(BubbleLaunchScreenOffset, height));
		
		// Рандомно выбираем скорость
		const float speed = math::random(MinBubbleSpeed, MaxBubbleSpeed);
		
		// Рандомно выбираем угол направления полета
		const float angle = math::random(1.f, 2 * math::PI - 1.f);
		
		const FPoint direction(math::cos(angle), math::sin(angle));

		// Создаем пузырь с заданными параметрами и углом поворота 0
		// Пузырь всегда повернут в одну и ту же сторону, вне зависимости от того, куда он летит
		
		const BubblePtr bubblePtr(new Bubble(position, 0.f, direction.Normalized(), speed));
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
	// Заполняем quadtree всеми объектами на сцене, кроме пушки и стен.
	// Пушка не участвует в определении столкновений, а стены проверяются вручную
	
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
