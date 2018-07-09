#pragma once
#include <boost/timer.hpp>

class QuadTree;

class Wall;
typedef std::unique_ptr<Wall> WallPtr;

class Projectile;
typedef boost::intrusive_ptr<Projectile> ProjectilePtr;

class Cannon;
typedef std::unique_ptr<Cannon> CannonPtr;

class Bubble;
typedef boost::intrusive_ptr<Bubble> BubblePtr;

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
	
	void StartNewGame();
	void PlayWinnerEffects();

	void UpdateCannon(float dt);

	void CheckAndResolveProjectilesCollisions(float dt, QuadTree& quadTree);
	void CheckAndResolveBubblesCollisions(float dt, QuadTree& quadTree);

	void DrawProjectiles();
	void LaunchProjectile(const IPoint& position);
	const FPoint CalculateProjectileStartPosition() const;
	void RemoveProjectile(const ProjectilePtr& projectile);
	
	void DrawBubbles();
	void LaunchBubbles();
	void RemoveBubble(const BubblePtr& projectile);

	void FillQuadTree(QuadTree& quad);

protected:
	static const float ProjectileSpeed;
	
	static const float MinBubbleSpeed;
	static const float MaxBubbleSpeed;
	
	static const float BubbleLaunchScreenOffset;
	
	static const int BubblesCount;
	
	static const int PlayTime;

protected:
	const FPoint _startPosition;
	const FRect _screenRect;

	CannonPtr _cannon;

	std::list<ProjectilePtr> _launchedProjectiles;
	std::list<BubblePtr> _bubbles;

	EffectsContainer _effCont;
	
	std::array<WallPtr, 4> _walls;

	size_t _projectilesTotalLaunch;

	boost::timer _timer_b;
	
	float _shading;
	
	Render::Texture* _background;
};
