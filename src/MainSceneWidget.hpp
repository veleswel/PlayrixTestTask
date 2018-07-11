#pragma once
#include "GameStateHandler.hpp"

class QuadTree;

class Wall;
typedef std::unique_ptr<Wall> WallPtr;

class Projectile;
typedef boost::intrusive_ptr<Projectile> ProjectilePtr;

class Cannon;
typedef std::unique_ptr<Cannon> CannonPtr;

class Bubble;
typedef boost::intrusive_ptr<Bubble> BubblePtr;

typedef std::unique_ptr<EffectsContainer> EffectsContainerPtr;

class MainSceneWidget: public GUI::Widget
{
public:
	MainSceneWidget(const std::string& name, rapidxml::xml_node<>* elem);
	
	void Draw() override;
	void Update(float dt) override;
	
	void AcceptMessage(const Message& message) override;

	bool MouseDown(const IPoint& mouse_pos) override;
	void MouseMove(const IPoint& mouse_pos) override;
	void MouseUp(const IPoint& mouse_pos) override;
	
	virtual void KeyPressed(int keyCode) override;
	
	static bool IsDebugDraw() { return false; }

protected:
	void Init();
	void ReadInitialData();
	
	template<typename T>
	T ReadLineAndGetValue(IO::TextReader* reader, const std::string& valueName)
	{
		std::string out, name, value;
		T result;
		
		out = reader->ReadAsciiLine();
		if (utils::ReadNvp(out, name, value) && name == valueName)
		{
			result = utils::lexical_cast<T>(value);
		}
		else
		{
			Log::Error("Failed to read data from file");
			assert(false);
		}
		
		return result;
	}
	
	void StartNewGame();
	void PauseGame();
	void ResumeGame();
	void Win();
	void Loose();
	void FinishGame();

	void DrawFinishImageAndText();
	void CalculateFinishImagePosition();
	
	void UpdateCannon(float dt);
	void UpdateGameItems(float dt);

	void CheckAndResolveProjectilesCollisions(float dt, QuadTree& quadTree);
	void CheckAndResolveBubblesCollisions(float dt, QuadTree& quadTree);

	void DrawProjectiles();
	void LaunchProjectile();
	const FPoint CalculateProjectileStartPosition() const;
	void RemoveProjectile(const ProjectilePtr& projectile);
	
	void DrawBubbles();
	void LaunchBubbles();
	void RemoveBubble(const BubblePtr& projectile);

	void FillQuadTree(QuadTree& quad);

protected:
	static const float MinBubbleSpeed;
	static const float MaxBubbleSpeed;
	
	static const float BubbleLaunchScreenOffset;
	
	static const float CannonPositionYOffset;
	static const float InfoTextXOffset;
	static const float LaunchedTextYOffset;
	static const float BubblesTextYOffset;
	static const float TimeTextYOffset;
	static const float FinishImageYOffset;
	static const float MenuEnterTextYOffset;
	
protected:
	float _projectileSpeed;
	int _startBubblesCount;
	float _playTime;
	
	const FPoint _startPosition;
	const FRect _screenRect;

	CannonPtr _cannon;

	std::list<ProjectilePtr> _launchedProjectiles;
	std::list<BubblePtr> _bubbles;

	EffectsContainerPtr _effectContainer;
	
	std::array<WallPtr, 4> _walls;

	size_t _projectilesTotalLaunched;

	Core::Timer _timer;
	float _timeLeft;

	Render::TexturePtr _background;
	Render::TexturePtr _gameFinishImage;
	IPoint _gameFinishPosition;
	
	GameStateHandler& _gameStateHandlerRef;
};
