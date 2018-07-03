#pragma once
#include "Cannon.hpp"
#include "Projectile.hpp"
#include "Bubble.hpp"

static const float Width = 10.f;

struct Wall
{
	FPoint _start;
	FPoint _end;
	
	math::Vector3 _vector;
	math::Vector3 _normal;
	
	FRect _bbox;
	
	Wall()
		: _start(0, 0)
		, _end(0, 0)
		, _normal(0, 0, 0)
		, _vector(0, 0, 0)
		, _bbox(0, 0, 0, 0)
	{
		
	}
	
	Wall(const Wall& wall)
		: _start(wall._start)
		, _end(wall._end)
		, _normal(wall._normal)
		, _vector(wall._vector)
		, _bbox(wall._bbox)
	{
		
	}
	
	Wall(const FPoint& start, const FPoint& end)
		: _start(start)
		, _end(end)
		, _normal(0, 0, 0)
		, _vector(0, 0, 0)
		, _bbox(0, 0, 0, 0)
	{
		CalculateVector();
		CalculateNormal();
		CalculateBBox();
	}
	
	Wall(float xStart, float yStart, float xEnd, float yEnd)
		: _start(FPoint(xStart, yStart))
		, _end(FPoint(xEnd, yEnd))
		, _normal(0, 0, 0)
		, _bbox(0, 0, 0, 0)
	{
		CalculateVector();
		CalculateNormal();
		CalculateBBox();
	}
	
	Wall& operator = (const Wall& wall)
	{
		if (this == &wall)
		{
			return *this;
		}
		
		_start = wall._start;
		_end = wall._end;
		_normal = wall._normal;
		_vector = wall._vector;
		_bbox = wall._bbox;
		
		return *this;
	}
	
	friend bool operator == (const Wall& left, const Wall& right)
	{
		return (left._start == right._start) && (left._end == right._end);
	}
	
	void CalculateVector()
	{
		const math::Vector3 start(_start.x, _start.y, 0.f);
		const math::Vector3 end(_end.x, _end.y, 0.f);
		
		_vector = end - start;
	}
	
	void CalculateNormal()
	{
		auto v = _vector.Normalized();
		
		_normal.x = -v.y + 0.f;
		_normal.y = v.x + 0.f;
		_normal.z = 0.f;
	}
	
	void CalculateBBox()
	{
		float xStart = _start.x;
		float xEnd = _end.x + _normal.x * Width;
		
		float yStart = _start.y;
		float yEnd = _end.y + _normal.y * Width;
		
		const float minX = math::min(xStart, xEnd);
		const float maxX = math::max(xStart, xEnd);
		
		const float minY = math::min(yStart, yEnd);
		const float maxY = math::max(yStart, yEnd);
		
		_bbox.xStart = minX;
		_bbox.xEnd = maxX;
		_bbox.yStart = minY;
		_bbox.yEnd = maxY;
	}
	
	const math::Vector3& GetVector() const
	{
		return _vector;
	}
	
	const math::Vector3& GetNormal() const
	{
		return _normal;
	}
	
	const FRect& GetBBox() const
	{
		return _bbox;
	}
	
	void Draw()
	{
		Render::device.SetTexturing(false);
		Render::BeginColor(Color(228, 237, 113, 255));
		Render::DrawRect(_bbox.xStart, _bbox.yStart, _bbox.Width(), _bbox.Height());
		Render::EndColor();
		Render::device.SetTexturing(true);
	}
};

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
	
	void KeyPressed(int keyCode) override;
	
protected:
	void Init();
	
	void UpdateCannon(float dt);
	
	void DrawProjectiles();
	void UpdateProjectiles(float dt);
	
	void LaunchProjectile(const IPoint& position);
	FPoint CalculateProjectileStartPosition() const;

	void DestroyProjectile(const ProjectilePtr& projectile);
	
	void DrawBubbles();
	void UpdateBubbles(float dt);
	
	void LaunchBubbles();

	static EColissionSide DoBoxesCollide(const FRect& bbox1, const FRect& bbox2);

protected:
	static const float ProjectileSpeed;
	
	static const float MinBubbleSpeed;
	static const float MaxBubbleSpeed;
	
	static const float BubbleLaunchScreenPrecision;
	
	static const int BubblesCount;
	
protected:
	const FPoint _startPosition;
	const FRect _screenRect;
	
	float _timer;

	CannonPtr _cannon;
	std::list<ProjectilePtr> _launchedProjectiles;
	std::list<BubblePtr> _bubbles;
	
	EffectsContainer _effCont;
	
	std::array<Wall, 4> _walls;
};
