#pragma once
#include "Cannon.hpp"
#include "Projectile.hpp"
#include "Bubble.hpp"
#include "QuadTree.hpp"

static const float Width = 10.f;

struct Wall
{
	FPoint _start;
	FPoint _end;
	
	math::Vector3 _vector;
	math::Vector3 _normal;
	
	OBB2D _obb;
	
	Wall()
		: _start(0, 0)
		, _end(0, 0)
		, _normal(0, 0, 0)
		, _vector(0, 0, 0)
		, _obb()
	{
		
	}
	
	Wall(const Wall& wall)
		: _start(wall._start)
		, _end(wall._end)
		, _normal(wall._normal)
		, _vector(wall._vector)
		, _obb(wall._obb)
	{
		
	}
	
	Wall(const FPoint& start, const FPoint& end)
		: _start(start)
		, _end(end)
		, _normal(0, 0, 0)
		, _vector(0, 0, 0)
		, _obb()
	{
		Init();
	}
	
	Wall(float xStart, float yStart, float xEnd, float yEnd)
		: _start(FPoint(xStart, yStart))
		, _end(FPoint(xEnd, yEnd))
		, _normal(0, 0, 0)
		, _obb()
	{
		Init();
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
		_obb = wall._obb;
		
		return *this;
	}
	
	friend bool operator == (const Wall& left, const Wall& right)
	{
		return (left._start == right._start) && (left._end == right._end);
	}
	
	void Init()
	{
		CalculateVector();
		CalculateNormal();
		CalculateOBB();
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
	
	void CalculateOBB()
	{
		float xStart = _start.x;
		float xEnd = _end.x + _normal.x * Width;
		
		float yStart = _start.y;
		float yEnd = _end.y + _normal.y * Width;
		
		const float minX = math::min(xStart, xEnd);
		const float maxX = math::max(xStart, xEnd);
		
		const float width = maxX - minX;
		
		const float minY = math::min(yStart, yEnd);
		const float maxY = math::max(yStart, yEnd);
		
		const float height = maxY - minY;
		
		const math::Vector3 center(minX + width / 2, minY + height / 2, 0);
		
		_obb = OBB2D(center, width, height, 0.f);
	}
	
	const math::Vector3& GetVector() const
	{
		return _vector;
	}
	
	const math::Vector3& GetNormal() const
	{
		return _normal;
	}
	
	const OBB2D& GetOBB() const
	{
		return _obb;
	}
	
	void Draw()
	{
		const auto& corner = _obb.GetCorners();

		Render::device.SetTexturing(false);
		Render::BeginColor(Color(0, 255, 255, 255));

		Render::DrawLine(corner[0], corner[1]);
		Render::DrawLine(corner[1], corner[2]);
		Render::DrawLine(corner[2], corner[3]);
		Render::DrawLine(corner[3], corner[0]);
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

	void DestroyBubble(const BubblePtr& projectile);

	void FillQuadTree(QuadTree& quad);

protected:
	static const float ProjectileSpeed;
	
	static const float MinBubbleSpeed;
	static const float MaxBubbleSpeed;
	
	static const float BubbleLaunchScreenPrecision;
	
	static const int BubblesCount;
	
protected:
	const FPoint _startPosition;
	const FRect _screenRect;

	CannonPtr _cannon;

	std::list<ProjectilePtr> _launchedProjectiles;
	std::list<BubblePtr> _bubbles;

	EffectsContainer _effCont;
	
	std::array<Wall, 4> _walls;
};
