#pragma once
#include "MovableObject.hpp"

typedef std::unique_ptr<EffectsContainer> EffectsContainerPtr;

class Projectile: public MovableObject
{
public:
	Projectile(const FPoint& position,
			   float rotation,
			   const FPoint& direction,
			   float speed,
			   const EffectsContainerPtr& container);
	
	virtual ~Projectile();

public:
	virtual void Draw() override;
	virtual void Update(float dt) override;

	virtual void SetPosition(const FPoint& position) override;
	virtual void SetPosition(float x, float y)  override;

	virtual void SetDirection(const FPoint& direction) override;
	virtual CollisionUtils::EColliderType GetColliderType() const override;

	virtual float GetOBBRotationAngle() const override { return _angle; }

	bool IsNeedToCheckCollision() const { return _isDrawn;  }

protected:
	void Init(const std::string& textureName,
			  const FPoint& position,
			  float rotation,
			  const FPoint& direction,
			  float speed,
			  const EffectsContainerPtr& container);

protected:
	static const std::string ProjectileTextureName;

protected:
	ParticleEffectPtr _effect;
	bool _isDrawn;
};

typedef boost::intrusive_ptr<Projectile> ProjectilePtr;
